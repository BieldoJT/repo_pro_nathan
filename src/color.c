/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gda-conc <gda-conc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 21:13:40 by gda-conc          #+#    #+#             */
/*   Updated: 2025/09/22 19:47:44 by gda-conc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static int	rr_terminate(t_vec3 *atten)
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
	t_pdf	pdf_cosine;
	t_pdf	pdf_light;
	t_pdf	pdf_mixed;
	t_vec3	sample_dir;
	t_ray	ray_scattered;
	double	pdf_sample_value;
	double	pdf_bsdf_value;
	t_vec3	radiance_child;
	t_vec3	radiance_estimate;
	void	*chosen_light;

	chosen_light = pick_emissive_sphere(rt);
	pdf_cosine = pdf_cosine_make(rec->normal);
	pdf_light = chosen_light ? pdf_light_sphere_make(chosen_light, rec->p)
		: pdf_cosine_make(rec->normal);
	pdf_mixed = pdf_mix_make(pdf_cosine, pdf_light);

	sample_dir = vec3_unit_vector(pdf_mixed.generate(&pdf_mixed));
	ray_scattered = ray(rec->p, sample_dir);
	pdf_sample_value = pdf_mixed.value(&pdf_mixed, sample_dir);

	radiance_estimate = vec3(0.0, 0.0, 0.0);
	if (pdf_sample_value > 0.0)
	{
		if (rec->material->scattering_pdf)
			pdf_bsdf_value = rec->material->scattering_pdf(rec->material, r_in, rec, &sample_dir);
		else
			pdf_bsdf_value = 1.0;
		radiance_child = ray_color(ray_scattered, rt, depth - 1);
		radiance_estimate = vec3_mul(radiance_child, pdf_bsdf_value / pdf_sample_value);
	}

	/* --- CLEANUP para evitar OOM --- */
	/* Importante: libere na ordem abaixo para não dar double-free */
	if (pdf_mixed.data)
		free(pdf_mixed.data);      /* free do 'struct s_pdfm' (wrapper do mix) */
	if (pdf_cosine.data)
		free(pdf_cosine.data);      /* free do 'struct s_pdfc' (cosine)        */
	if (pdf_light.data)
		free(pdf_light.data);      /* free do 'struct s_pdfls' OU 's_pdfc'    */
	return (radiance_estimate);
}

t_vec3	ray_color(t_ray r, t_rt *rt, int depth)
{
	t_hit_record		hit;
	t_scatter_params	scatter_params;
	t_vec3				atten;
	t_ray				ray_next;
	t_vec3				emission;
	t_vec3				indirect_radiance;

	scatter_params.r_in = &r;
	scatter_params.rec = &hit;
	scatter_params.attenuation = &atten;
	scatter_params.scattered = &ray_next;
	scatter_params.is_specular = 0;
	scatter_params.pdf = 0.0;
	if (depth <= 0)
		return (vec3(0.0, 0.0, 0.0));
	if (!hit_world(r, &hit, rt))
		return (vec3(0.0, 0.0, 0.0));
	/* emissão: seu t_material não tem emitted_fn; use color_emited do próprio material */
	emission = hit.material->color_emited;
	if (!hit.material->scatter(hit.material, &scatter_params))
		return (emission);
	/* Russian Roulette (se você já tem rr_terminate, mantenha; senão remova este if) */
	if (depth <= 5 && rr_terminate(&atten))
		return (emission);
	/* Specular: segue direto sem PDF */
	if (scatter_params.is_specular)
		return (vec3_add(emission, vec3_mult_vecs(atten, ray_color(ray_next, rt, depth - 1))));
	/* Difuso: MIS (cosine + esfera de luz) */
	indirect_radiance = estimate_mis(rt, &r, &hit, depth);
	return (vec3_add(emission, vec3_mult_vecs(atten, indirect_radiance)));
}

