/*
 * reorient_rt: Rotate triangle meshes in a .rt file by Euler angles.
 *
 * Usage:
 *   reorient_rt <input.rt> <output.rt> <deg_x> <deg_y> <deg_z>
 *
 * Behavior:
 * - Parses the input .rt line by line.
 * - For lines starting with 'tr', rotates the three vertex vectors by the
 *   rotation R = Rz * Ry * Rx (angles provided in degrees).
 * - All other lines are written unchanged.
 * - Lines starting with "# Bounds" (from our tooling) are skipped from input,
 *   and updated bounds for the rotated triangles are appended at the end.
 *
 * Notes:
 * - Rotation is around the origin (0,0,0). If your object isn't centered at
 *   the origin, use move_rt to translate to origin, rotate with this tool, and
 *   then move back.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct s_vec3 {
	double x, y, z;
} t_vec3;

static void die_usage(const char *msg)
{
	if (msg && *msg)
		fprintf(stderr, "Error: %s\n", msg);
	fprintf(stderr, "Usage: reorient_rt <input.rt> <output.rt> <deg_x> <deg_y> <deg_z>\n");
	exit(EXIT_FAILURE);
}

static bool starts_with_trim(const char *s, const char *prefix)
{
	while (*s == ' ' || *s == '\t') s++;
	size_t n = strlen(prefix);
	return strncmp(s, prefix, n) == 0;
}

static bool is_triangle_line(const char *line)
{
	// Trim leading whitespace, then expect "tr " or "tr\t"
	while (*line == ' ' || *line == '\t') line++;
	return (line[0] == 't' && line[1] == 'r' && (line[2] == ' ' || line[2] == '\t'));
}

static int split_whitespace(char *str, char *out[], int max_tokens)
{
	int count = 0;
	char *tok = strtok(str, " \t\r\n");
	while (tok) {
		if (count < max_tokens) {
			out[count++] = tok;
		} else {
			break;
		}
		tok = strtok(NULL, " \t\r\n");
	}
	return count;
}

static bool parse_vec3_token(const char *tok, t_vec3 *out)
{
	// Expected format: x,y,z (commas separating floats)
	char *end;
	errno = 0;
	double x = strtod(tok, &end);
	if (errno != 0 || !end || *end != ',') return false;
	const char *p = end + 1;
	errno = 0;
	double y = strtod(p, &end);
	if (errno != 0 || !end || *end != ',') return false;
	p = end + 1;
	errno = 0;
	double z = strtod(p, &end);
	if (errno != 0) return false;
	// Allow trailing characters only if they are string terminators
	if (*end != '\0') return false;
	out->x = x; out->y = y; out->z = z;
	return true;
}

static void rotate_build_matrix(double rx_deg, double ry_deg, double rz_deg, double R[3][3])
{
	const double rx = rx_deg * (M_PI / 180.0);
	const double ry = ry_deg * (M_PI / 180.0);
	const double rz = rz_deg * (M_PI / 180.0);
	const double cx = cos(rx), sx = sin(rx);
	const double cy = cos(ry), sy = sin(ry);
	const double cz = cos(rz), sz = sin(rz);

	// Rx
	double Rx[3][3] = {
		{1, 0, 0},
		{0, cx, -sx},
		{0, sx, cx}
	};
	// Ry
	double Ry[3][3] = {
		{cy, 0, sy},
		{0, 1, 0},
		{-sy, 0, cy}
	};
	// Rz
	double Rz[3][3] = {
		{cz, -sz, 0},
		{sz, cz, 0},
		{0, 0, 1}
	};

	// tmp = Ry * Rx
	double tmp[3][3];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			tmp[i][j] = 0.0;
			for (int k = 0; k < 3; ++k)
				tmp[i][j] += Ry[i][k] * Rx[k][j];
		}
	}
	// R = Rz * (Ry * Rx)
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			R[i][j] = 0.0;
			for (int k = 0; k < 3; ++k)
				R[i][j] += Rz[i][k] * tmp[k][j];
		}
	}
}

static t_vec3 rotate_apply(const double R[3][3], t_vec3 v)
{
	t_vec3 r;
	r.x = R[0][0]*v.x + R[0][1]*v.y + R[0][2]*v.z;
	r.y = R[1][0]*v.x + R[1][1]*v.y + R[1][2]*v.z;
	r.z = R[2][0]*v.x + R[2][1]*v.y + R[2][2]*v.z;
	return r;
}

static void bbox_init(t_vec3 *minv, t_vec3 *maxv)
{
	minv->x = minv->y = minv->z = 1e300;
	maxv->x = maxv->y = maxv->z = -1e300;
}

static void bbox_expand(t_vec3 *minv, t_vec3 *maxv, t_vec3 p)
{
	if (p.x < minv->x) { minv->x = p.x; }
	if (p.x > maxv->x) { maxv->x = p.x; }
	if (p.y < minv->y) { minv->y = p.y; }
	if (p.y > maxv->y) { maxv->y = p.y; }
	if (p.z < minv->z) { minv->z = p.z; }
	if (p.z > maxv->z) { maxv->z = p.z; }
}

static void write_vec3_token(char *dst, size_t sz, t_vec3 v)
{
	// 6 decimals to match other tools
	snprintf(dst, sz, "%.6f,%.6f,%.6f", v.x, v.y, v.z);
}

int main(int argc, char **argv)
{
	if (argc != 6)
		die_usage("invalid number of arguments");

	const char *in_path = argv[1];
	const char *out_path = argv[2];
	char *end;
	errno = 0; double rx = strtod(argv[3], &end); if (errno || *end!='\0') die_usage("deg_x must be a number");
	errno = 0; double ry = strtod(argv[4], &end); if (errno || *end!='\0') die_usage("deg_y must be a number");
	errno = 0; double rz = strtod(argv[5], &end); if (errno || *end!='\0') die_usage("deg_z must be a number");

	FILE *fin = fopen(in_path, "r");
	if (!fin) { perror("fopen input"); return EXIT_FAILURE; }
	FILE *fout = fopen(out_path, "w");
	if (!fout) { perror("fopen output"); fclose(fin); return EXIT_FAILURE; }

	double R[3][3];
	rotate_build_matrix(rx, ry, rz, R);

	char line[8192];
	t_vec3 bb_min, bb_max; bbox_init(&bb_min, &bb_max);
	int tri_count = 0;

	while (fgets(line, sizeof(line), fin)) {
		// Skip our previous bounds comments
		if (starts_with_trim(line, "# Bounds")) {
			continue;
		}

		if (!is_triangle_line(line)) {
			fputs(line, fout);
			continue;
		}

		// Tokenize
		char work[8192];
		strncpy(work, line, sizeof(work)-1);
		work[sizeof(work)-1] = '\0';
		char *toks[128];
		int n = split_whitespace(work, toks, 128);
		if (n < 4 || strcmp(toks[0], "tr") != 0) {
			// If malformed, just write as-is to avoid data loss
			fputs(line, fout);
			continue;
		}

		t_vec3 v1, v2, v3;
		if (!parse_vec3_token(toks[1], &v1) || !parse_vec3_token(toks[2], &v2) || !parse_vec3_token(toks[3], &v3)) {
			// Malformed triangle line; write as-is
			fputs(line, fout);
			continue;
		}

		v1 = rotate_apply(R, v1);
		v2 = rotate_apply(R, v2);
		v3 = rotate_apply(R, v3);
		bbox_expand(&bb_min, &bb_max, v1);
		bbox_expand(&bb_min, &bb_max, v2);
		bbox_expand(&bb_min, &bb_max, v3);
		tri_count++;

		char v1s[128], v2s[128], v3s[128];
		write_vec3_token(v1s, sizeof(v1s), v1);
		write_vec3_token(v2s, sizeof(v2s), v2);
		write_vec3_token(v3s, sizeof(v3s), v3);

		// Reconstruct line: "tr v1 v2 v3" + rest tokens (color or any extra)
		fputs("tr ", fout);
		fputs(v1s, fout); fputc(' ', fout);
		fputs(v2s, fout); fputc(' ', fout);
		fputs(v3s, fout);
		for (int i = 4; i < n; ++i) {
			fputc(' ', fout);
			fputs(toks[i], fout);
		}
		fputc('\n', fout);
	}

	if (tri_count > 0) {
		// Append updated bounds comments
		fprintf(fout, "# Bounds min %.6f %.6f %.6f\n", bb_min.x, bb_min.y, bb_min.z);
		fprintf(fout, "# Bounds max %.6f %.6f %.6f\n", bb_max.x, bb_max.y, bb_max.z);
	}

	fclose(fin);
	fclose(fout);
	return EXIT_SUCCESS;
}

