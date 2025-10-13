#ifndef VEC3_H
# define VEC3_H

typedef struct s_vec3
{
	float	x;
	float	y;
	float	z;
}	t_vec3;

// Constructors
t_vec3	v3(float x, float y, float z);

// Arithmetic
t_vec3	v3_add(t_vec3 a, t_vec3 b);
t_vec3	v3_sub(t_vec3 a, t_vec3 b);
t_vec3	v3_mul(t_vec3 a, float s);
t_vec3	v3_div(t_vec3 a, float s);

// Dot/Cross and length
float	v3_dot(t_vec3 a, t_vec3 b);
t_vec3	v3_ctoc(t_vec3 a, t_vec3 b);
t_vec3	v3_cross(t_vec3 a, t_vec3 b);
float	v3_len2(t_vec3 a);
float	v3_len(t_vec3 a);
t_vec3	v3_norm(t_vec3 a);

#endif