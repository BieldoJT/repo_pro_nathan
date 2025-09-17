#include "rt.h"

static double	value_m(t_pdf *p, t_vec3 d)
{
	t_pdfm	*m;

	m = (t_pdfm *)p->data;
	return (0.5 * m->a.value(&m->a, d) + 0.5 * m->b.value(&m->b, d));
}

static t_vec3	generate_m(t_pdf *p)
{
	t_pdfm	*m;

	m = (t_pdfm *)p->data;
	if (random_double() < 0.5)
		return (m->a.generate(&m->a));
	return (m->b.generate(&m->b));
}

t_pdf	pdf_mix_make(t_pdf a, t_pdf b)
{
	t_pdf	p;
	t_pdfm	*m;

	m = (t_pdfm *)malloc(sizeof(t_pdfm));
	if (!m)
		return ((t_pdf){0});
	m->a = a;
	m->b = b;
	p.data = m;
	p.value = &value_m;
	p.generate = &generate_m;
	return (p);
}
