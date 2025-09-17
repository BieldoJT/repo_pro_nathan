#include "../rt.h"
#include "../ray.h"

double	sphere_pdf_value_from(void *sp, t_vec3 origin, t_vec3 dir);
t_vec3	sphere_random_dir_to(void *sp, t_vec3 origin);

typedef struct s_pdfls
{
	void	*sphere;
	t_vec3	origin;
}	t_pdfls;

static double	value_ls(t_pdf *p, t_vec3 d)
{
	t_pdfls	*l;

	l = (t_pdfls *)p->data;
	return (sphere_pdf_value_from(l->sphere, l->origin, d));
}

static t_vec3	generate_ls(t_pdf *p)
{
	t_pdfls	*l;

	l = (t_pdfls *)p->data;
	return (sphere_random_dir_to(l->sphere, l->origin));
}

t_pdf	pdf_light_sphere_make(void *sphere, t_vec3 origin)
{
	t_pdf	p;
	t_pdfls	*l;

	l = (t_pdfls *)malloc(sizeof(t_pdfls));
	if (!l)
		return ((t_pdf){0});
	l->sphere = sphere;
	l->origin = origin;
	p.data = l;
	p.value = &value_ls;
	p.generate = &generate_ls;
	return (p);
}
