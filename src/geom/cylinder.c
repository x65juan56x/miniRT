#include "../../include/minirt.h"
#include "../../include/hit.h"


int inside_cyl(const t_cyl *cylinder, t_vec3 p, t_vec3 v)
{
	float height_pos = v3_dot(v3_sub(p, cylinder->center), v);
	if ((height_pos > 0 && height_pos <= - cylinder->he/2) || (height_pos < 0 && height_pos >= - cylinder->he/2))
		return 1;
	return 0;
}

float	hit_cylinder(const t_cyl *cylinder, t_ray r)
{
	t_vec3		v;
	t_vec3		d;
	t_vec3		x;
	// x = (P - C);
	x = v3_sub(r.orig, cylinder->center);
	//v = vector del eje normalizado
	v = cylinder->axis;
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

	// Paso 5: Elegir la mas pequeña??
	if(t1 > 0)
		t = t1;
	if (t2 > 0 && t2 < t)
		t = t2;
	if(t < 0)
		return -1.0f;
	// Paso 6 = calcular punto de impacto
	
	t_vec3 p = v3_add(r.orig, v3_mul(r.dir, t));

	// Paso 8 (opcional): limitar por la altura del cilindro
	//condicion de tapas, si es menor que el radio y es perpendicular al punto de la base
	//cylinder->center no es la base

	if (inside_cyl(cylinder, p, v))
		return(t);
	return -1.0f;
}


