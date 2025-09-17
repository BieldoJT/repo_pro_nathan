#include "../rt.h"
#include "../ray.h"

static double	len2(t_vec3 v)
{
	return (v.x * v.x + v.y * v.y + v.z * v.z);
}

double	sphere_pdf_value_from(void *spv, t_vec3 o, t_vec3 v)
{
	t_sphere	*sp;
	t_vec3		oc;
	double		d2;
	double		r2;
	double		cos_max;
	double		solid;

	sp = (t_sphere *)spv;
	v = vec3_unit_vector(v);
	oc = vec3_sub(sp->center, o);
	d2 = len2(oc);
	r2 = sp->radius * sp->radius;
	if (d2 <= r2)
		return (0.0);
	cos_max = sqrt(1.0 - r2 / d2);
	solid = 2.0 * PI * (1.0 - cos_max);
	return (1.0 / solid);
}



t_vec3	sphere_random_dir_to(void *spv, t_vec3 o)
{
	t_sphere	*sp;
	t_vec3		oc;
	double		d2;
	double		r2;
	double		cm;
	double		r1;
	double		r2u;
	double		z;
	double		phi;
	double		x;
	double		y;
	t_onb		b;
	t_vec3		w;

	sp = (t_sphere *)spv;
	oc = vec3_sub(sp->center, o);
	d2 = len2(oc);
	r2 = sp->radius * sp->radius;
	w = vec3_unit_vector(oc);
	onb_build(&b, w);
	cm = sqrt(1.0 - r2 / d2);
	r1 = random_double();
	r2u = random_double();
	z = 1.0 - r2u * (1.0 - cm);
	phi = 2.0 * PI * r1;
	x = cos(phi) * sqrt(1.0 - z * z);
	y = sin(phi) * sqrt(1.0 - z * z);
	return (onb_local(&b, x, y, z));
}
