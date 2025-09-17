#include "rt.h"


static double	value_c(t_pdf *p, t_vec3 d)
{
	double	c;

	(void)p;
	d = vec3_unit_vector(d);
	c = d.z;
	if (c <= 0.0)
		return (0.0);
	return (c /PI);
}

static t_vec3	generate_c(t_pdf *p)
{
	double	r1;
	double	r2;
	double	z;
	double	phi;
	double	x;
	double	y;
	t_pdfc	*c;

	c = (t_pdfc *)p->data;
	r1 = random_double();
	r2 = random_double();
	z = sqrt(1.0 - r2);
	phi = 2.0 * PI * r1;
	x = cos(phi) * sqrt(r2);
	y = sin(phi) * sqrt(r2);
	return (onb_local(&c->basis, x, y, z));
}

t_pdf	pdf_cosine_make(t_vec3 normal)
{
	t_pdf	p;
	t_pdfc	*c;

	c = (t_pdfc *)malloc(sizeof(t_pdfc));
	if (!c)
		return ((t_pdf){0});
	onb_build(&c->basis, normal);
	p.data = c;
	p.value = &value_c;
	p.generate = &generate_c;
	return (p);
}
