#include "rt.h"

void	onb_build(t_onb *b, t_vec3 n)
{
	t_vec3	a;

	b->w = vec3_unit_vector(n);
	a = (fabs(b->w.x) > 0.9) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	b->v = vec3_unit_vector(vec3_cross(b->w, a));
	b->u = vec3_cross(b->w, b->v);
}

t_vec3	onb_local(const t_onb *b, double a, double b2, double c)
{
	t_vec3	x;
	t_vec3	y;
	t_vec3	z;

	x = vec3_mul(b->u, a);
	y = vec3_mul(b->v, b2);
	z = vec3_mul(b->w, c);
	return (vec3_add(vec3_add(x, y), z));
}

