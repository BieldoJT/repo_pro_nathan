/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bieldojt <bieldojt@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 21:13:40 by gda-conc          #+#    #+#             */
/*   Updated: 2025/09/17 11:10:14 by bieldojt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static int rr_terminate(t_vec3 *atten)
{
	double  probability;

	/* prob. de sobrevivência baseada no maior canal da atenuação (clamp) */
	probability = atten->x;
	if (atten->y > probability)
		probability = atten->y;
	if (atten->z > probability)
		probability = atten->z;
	if (probability < 0.1)
		probability = 0.1;
	if (random_double() > probability)
	    return (1);
	*atten = vec3_div(*atten, probability);
	return (0);
}

static void	*pick_emissive_sphere(t_rt *rt)
{
	int			i;
	t_sphere	*sp;
	t_vec3		e;

	i = 0;
	while (i < rt->world_size)
	{
		sp = (t_sphere *)rt->world[i]->obj;
		if (sp && sp->material)
		{
			e = sp->material->color_emited;
			if (e.x > 0.0 || e.y > 0.0 || e.z > 0.0)
				return ((void *)sp);
		}
		i++;
	}
	return (NULL);
}

/* 1 amostra MIS: mistura cosine + esfera emissiva (se houver) */
static t_vec3	estimate_mis(t_rt *rt, const t_ray *r_in,
				const t_hit_record *rec, int depth)
{
	t_pdf	pc;
	t_pdf	pl;
	t_pdf	pm;
	t_vec3	wi;
	t_ray	r_out;
	double	pdfv;
	double	spdf;
	t_vec3	col;
	t_vec3	out;
	void	*light;

	light = pick_emissive_sphere(rt);
	pc = pdf_cosine_make(rec->normal);
	pl = light ? pdf_light_sphere_make(light, rec->p)
		: pdf_cosine_make(rec->normal);
	pm = pdf_mix_make(pc, pl);

	wi = vec3_unit_vector(pm.generate(&pm));
	r_out = ray(rec->p, wi);
	pdfv = pm.value(&pm, wi);

	out = vec3(0.0, 0.0, 0.0);
	if (pdfv > 0.0)
	{
		if (rec->material->scattering_pdf)
			spdf = rec->material->scattering_pdf(rec->material, r_in, rec, &wi);
		else
			spdf = 1.0;
		col = ray_color(r_out, rt, depth - 1);
		out = vec3_mul(col, spdf / pdfv);
	}

	/* --- CLEANUP para evitar OOM --- */
	/* Importante: libere na ordem abaixo para não dar double-free */
	if (pm.data)
		free(pm.data);      /* free do 'struct s_pdfm' (wrapper do mix) */
	if (pc.data)
		free(pc.data);      /* free do 'struct s_pdfc' (cosine)        */
	if (pl.data)
		free(pl.data);      /* free do 'struct s_pdfls' OU 's_pdfc'    */

	return (out);
}

t_vec3	ray_color(t_ray r, t_rt *rt, int depth)
{
	t_hit_record		rec;
	t_scatter_params	p;
	t_vec3				atten;
	t_ray				sc;
	t_vec3				em;
	t_vec3				indir;

	p.r_in = &r;
	p.rec = &rec;
	p.attenuation = &atten;
	p.scattered = &sc;
	p.is_specular = 0;
	p.pdf = 0.0;
	if (depth <= 0)
		return (vec3(0.0, 0.0, 0.0));
	if (!hit_world(r, &rec, rt))
		return (vec3(0.0, 0.0, 0.0));
	/* emissão: seu t_material não tem emitted_fn; use color_emited do próprio material */
	em = rec.material->color_emited;
	if (!rec.material->scatter(rec.material, &p))
		return (em);
	/* Russian Roulette (se você já tem rr_terminate, mantenha; senão remova este if) */
	if (depth <= 5 && rr_terminate(&atten))
		return (em);
	/* Specular: segue direto sem PDF */
	if (p.is_specular)
		return (vec3_add(em, vec3_mult_vecs(atten, ray_color(sc, rt, depth - 1))));
	/* Difuso: MIS (cosine + esfera de luz) */
	indir = estimate_mis(rt, &r, &rec, depth);
	return (vec3_add(em, vec3_mult_vecs(atten, indir)));
}

