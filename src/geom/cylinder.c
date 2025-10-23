#include "../../include/minirt.h"
#include "../../include/hit.h"


int inside_cyl_height(const t_cyl *cylinder, t_vec3 p, t_vec3 v)
{
	float height_pos = v3_dot(v3_sub(p, cylinder->center), v);
	if ((fabsf(height_pos) <= cylinder->he * 0.5f))
		return 1;
	return 0;
}

int inside_cyl(const t_cyl *cylinder, t_vec3 p, t_vec3 v)
{
	t_vec3 radial;
	//radial = p - center;
	//p = punto extern
	//center = base + centro del cilindro;
	//radidal = vector que va desde el centro del cilindro hacia el punto p;
	float radio = cylinder->di/2;
	radial = v3_sub(p, cylinder->center);
	
	//Proyecta el vector radial sobre el eje del cilindro (axis).
	float axial = v3_dot(radial, v);
	radial = v3_sub(radial, v3_mul(v, axial));
	if (v3_dot(radial, radial) < radio * radio)
		return (1);
	else
		return 0;
}

int part_hitten(const t_cyl *cylinder, float t_side, float t_cap, float t_bottom, float t_final)
{
	int hit_part = -1;

	if (t_final == t_cap)
		hit_part = 1;
	else if (t_final == t_bottom)
		hit_part = 2;
	else if (t_final == t_side)
		hit_part = 0;

	return(hit_part);
}

float pick_smallest_positive(const t_cyl *cylinder, float t_side, float t_top, float t_bottom)
{
	float t_final = 1e30;
	if(t_top > 0)
		t_final = t_top;
	if (t_bottom > 0 && t_bottom < t_final)
		t_final = t_bottom;
	if (t_side > 0 && t_side < t_final)
		t_final = t_side;
	return(t_final);
}
float hit_top_cap(const t_cyl *cylinder, t_ray r, t_vec3 v)
{
	float t;
	float denom = v3_dot(r.dir, v);
	t_vec3 p;
	if (fabsf(denom) < 1e-6f)
    	return (-1.0f); //paralelo no hay interseccion
	t_vec3 c_top = v3_add(cylinder->center, v3_mul(v ,(cylinder->he * 0.5f)));

	t = v3_dot(v3_sub(c_top, r.orig), v) / denom;
	p = v3_add(r.orig, v3_mul(r.dir, t));
	float radio = cylinder->di * 0.5f;
	t_vec3 radial = v3_sub(p, c_top);
	//𝑃=O+tD.
	//∥P−Ctop​∥2≤r2
	if(t < 0.0f)
		return (-1.0f);
	if(v3_dot(radial, radial) <= radio * radio)
		return t;
	return (-1.0f);
}

float hit_botton_cap(const t_cyl *cylinder, t_ray r, t_vec3 v)
{
	float t;
	float denom = v3_dot(r.dir, v3_mul(v, -1.0f));
	t_vec3 p;
	if (fabsf(denom) < 1e-6f)
    	return (-1.0f); //paralelo no hay interseccion
	t_vec3 c_bottom = v3_sub(cylinder->center, v3_mul(v ,(cylinder->he * 0.5f)));

	t = v3_dot(v3_sub(c_bottom, r.orig), v) / denom;
	p = v3_add(r.orig, v3_mul(r.dir, t));
	float radio = cylinder->di * 0.5f;
	t_vec3 radial = v3_sub(p, c_bottom);
	//𝑃=O+tD.
	//∥P−Ctop​∥2≤r2
	if(t < 0.0f)
		return (-1.0f);
	if(v3_dot(radial, radial) <= radio * radio)
		return t;
	return (-1.0f);
}

float hit_side(const t_cyl *cylinder, t_ray r, t_vec3 v)
{
	t_vec3		v;
	t_vec3		d;
	t_vec3		x;
	// x = (P - C);
	x = v3_sub(r.orig, cylinder->center);
	//v = vector del eje normalizado
	v = v3_norm(cylinder->axis);
	// direccion del rayo
	d = r.dir;

	// Paso 2: Calcular proyecciones sobre el eje del cilindro
	//(P - C * v) = x * v
	float x_dot_v = v3_dot(x, v);
	float d_dot_v = v3_dot(d, v);

	//  Paso 3: Calcular proyecciones sobre el eje del cilindro
	float a;
	float b;
	float c;
	a = v3_dot(v3_sub(d, v3_mul(v, d_dot_v)), v3_sub(d, v3_mul(v, d_dot_v)));
	b = 2 * v3_dot(v3_sub(d, v3_mul(v, d_dot_v)), v3_sub(x, v3_mul(v, x_dot_v)));
	c = (v3_dot(v3_sub(x, v3_mul(v, x_dot_v)), v3_sub(x, v3_mul(v, x_dot_v)))) - (cylinder->di/2 * cylinder->di/2);
	
	// Paso 4: Discriminante
	float disc = (b * b) - (4 * a * c);
	if(disc < 0)
		return (-1); // no intersecta

	float t1 = (-b - sqrt(disc)) / (2*a);
	float t2 = (-b + sqrt(disc)) / (2*a);
	float t = 1e30;

	if(t < 0.0f)
		return (-1.0f);
	// Paso 5: Elegir la mas pequeña de side (necesito una funcion)??
	if(t1 > 0)
		t = t1;
	if (t2 > 0 && t2 < t)
		t = t2;
	return(t);
}

float	hit_cylinder(const t_cyl *cylinder, t_ray r, int *hit_part)
{

	float t_side = hit_side(cylinder, r, v);
	float t_top = hit_top_cap(cylinder, r, v);
	float t_bottom = hit_botton_cap(cylinder, r, v);
	float t_final = pick_smallest_positive(cylinder, t_side, t_top, t_bottom);
	int hit_part = part_hitten(cylinder, t_side, t_top, t_bottom, t_final)
/
* 	if(t < 0.0f)
		return -1.0f; */
	// Paso 6 = calcular punto de impacto
	
	t_vec3 p = v3_add(r.orig, v3_mul(r.dir, t));

	// Paso 8 (opcional): limitar por la altura del cilindro
	//condicion de tapas, si es menor que el radio y es perpendicular al punto de la base
	//cylinder->center no es la base

	if (inside_cyl_height(cylinder, p, v3_norm(v))/*  && inside_cyl(cylinder, p, v) */)
		return(t_final);
	/* if(inside_cyl (cylinder, p, v))
		return(t2); */
	return -1.0f;

	/*Calcular t_side (ya lo tienes).
	Calcular t_cap_top y t_cap_bottom.
	Escoger el menor t > 0 de los tres.
	Guardar cuál tipo de impacto fue (side, top, bottom) → para luego saber la normal correcta.*/
}

float hit_top_cap(const t_cyl *cylinder, t_ray r, t_vec3 v)
{
	float t;
	float denom = v3_dot(r.dir, v);
	t_vec3 p;
	if (fabsf(denom) < 1e-6f)
    	return (-1.0f); //paralelo no hay interseccion
	t_vec3 c_top = v3_add(cylinder->center, v3_mul(v ,(cylinder->he * 0.5f)));

	t = v3_dot(v3_sub(c_top, r.orig), v) / denom;
	p = v3_add(r.orig, v3_mul(r.dir, t));
	float radio = cylinder->di * 0.5f;
	t_vec3 radial = v3_sub(p, c_top);
	//𝑃=O+tD.
	//∥P−Ctop​∥2≤r2
	if(t < 0.0f)
		return (-1.0f);
	if(v3_dot(radial, radial) <= radio * radio)
		return t;
	return (-1.0f);
}

float hit_botton_cap(const t_cyl *cylinder, t_ray r, t_vec3 v)
{
	float t;
	float denom = v3_dot(r.dir, v3_mul(v, -1.0f));
	t_vec3 p;
	if (fabsf(denom) < 1e-6f)
    	return (-1.0f); //paralelo no hay interseccion
	t_vec3 c_bottom = v3_sub(cylinder->center, v3_mul(v ,(cylinder->he * 0.5f)));

	t = v3_dot(v3_sub(c_bottom, r.orig), v) / denom;
	p = v3_add(r.orig, v3_mul(r.dir, t));
	float radio = cylinder->di * 0.5f;
	t_vec3 radial = v3_sub(p, c_bottom);
	//𝑃=O+tD.
	//∥P−Ctop​∥2≤r2
	if(t < 0.0f)
		return (-1.0f);
	if(v3_dot(radial, radial) <= radio * radio)
		return t;
	return (-1.0f);
}







