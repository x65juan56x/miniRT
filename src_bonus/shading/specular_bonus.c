#include "../../include_bonus/minirt_bonus.h"

//Ispecular​=ks​⋅Ilight​⋅max(0,N⋅H)α
t_vec3 specular_blinn_phong(const t_scene *scene, const t_hit *hit, t_material *material)
{

	if (!material || !hit->ok)
		return v3(0.0f, 0.0f, 0.0f);
	//t_vec3 p = hit->p;
	t_vec3 n = v3_norm(hit->n); //
	t_vec3 l = v3_norm(v3_sub(scene->light.pos, hit->p)); //direccion hacia la luz
	t_vec3 v = v3_norm(v3_sub(scene->camera.pos, hit->p)); // direccion hacia la camara
	t_vec3 h = v3_norm(v3_add(l, v));
	//t_vec3 albedo =  material.albedo;
	float ks = material->ks;
	float shininess = material->shininess;

	//ks * (scene->light.pos * (n*h))shinniness;
	//v3_dot(n, h); mide cuanto el vector halfway se alinea con la normal, cuanto mayor es mas fuerte es el reflejo
	//n⋅h=cos(θ)
	//donde θ es el ángulo entre la normal y el vector halfway
	t_vec3 light_intensity = v3_mul(scene->light.color, scene->light.bright);
	float spec_angle = v3_dot(n, h);
	if(spec_angle < 0.0f)
		spec_angle = 0.0f;
	float spec_intensity = powf(spec_angle, shininess);
	//printf("ks: %f, shininess: %f\n", ks, shininess);
	//printf("spec_angle: %f, spec_intensity: %f\n", spec_angle, spec_intensity);
	t_vec3 spec_color = v3_mul(v3_mul(light_intensity, spec_intensity), ks);
	return (spec_color);
}
