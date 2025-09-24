/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gda-conc <gda-conc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 21:13:40 by gda-conc          #+#    #+#             */
/*   Updated: 2025/09/23 22:47:18 by gda-conc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static int	rr_terminate(t_vec3 *atten)
{
	double	probability;

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

static void	get_values_from_pdf(t_rt *rt, t_hit_record *rec, t_mis_data *data)
{
	data->chosen_light = pick_emissive_sphere(rt);
	data->pdf_cosine = pdf_cosine_make(rec->normal);
	if (data->chosen_light)
		data->pdf_light = pdf_light_sphere_make(data->chosen_light, rec->p);
	else
		data->pdf_light = pdf_cosine_make(rec->normal);
	data->pdf_mix = pdf_mix_make(data->pdf_cosine, data->pdf_light);
	data->sample_d = vec3_unit_vector(data->pdf_mix.generate(&data->pdf_mix));
	data->ray_scattered = ray(rec->p, data->sample_d);
	data->pdf_sample_value = data->pdf_mix.value(&data->pdf_mix,
			data->sample_d);
}

static t_vec3	estimate_mis(t_rt *rt, const t_ray *r_in,
				const t_hit_record *rec, int depth)
{
	t_mis_data		*data;
	t_vec3			radiance_estimate;

	data = (t_mis_data *)malloc(sizeof(t_mis_data));
	get_values_from_pdf(rt, (t_hit_record *)rec, data);
	radiance_estimate = vec3(0.0, 0.0, 0.0);
	if (data->pdf_sample_value > 0.0)
	{
		if (rec->material->scattering_pdf)
			data->pdf_bsdf_value = rec->material->scattering_pdf(rec->material,
					r_in, rec, &data->sample_d);
		else
			data->pdf_bsdf_value = 1.0;
		data->radiance_child = ray_color(data->ray_scattered, rt, depth - 1);
		radiance_estimate = vec3_mul(data->radiance_child, data->pdf_bsdf_value
				/data->pdf_sample_value);
	}
	if (data->pdf_mix.data)
		free(data->pdf_mix.data);
	if (data->pdf_cosine.data)
		free(data->pdf_cosine.data);
	if (data->pdf_light.data)
		free(data->pdf_light.data);
	free(data);
	return (radiance_estimate);
}

t_vec3	ray_color(t_ray r, t_rt *rt, int depth)
{
	t_trace_data	td;

	td.scatter_params.r_in = &r;
	td.scatter_params.rec = &td.hit;
	td.scatter_params.attenuation = &td.atten;
	td.scatter_params.scattered = &td.ray_next;
	td.scatter_params.is_specular = 0;
	td.scatter_params.pdf = 0.0;
	if (depth <= 0)
		return (vec3(0.0, 0.0, 0.0));
	if (!hit_world(r, &td.hit, rt))
		return (vec3(0.0, 0.0, 0.0));
	td.emission = td.hit.material->color_emited;
	if (!td.hit.material->scatter(td.hit.material, &td.scatter_params))
		return (td.emission);
	if (depth <= 5 && rr_terminate(&td.atten))
		return (td.emission);
	if (td.scatter_params.is_specular)
		return (vec3_add(td.emission, vec3_mult_vecs(td.atten,
					ray_color(td.ray_next, rt, depth - 1))));
	td.indirect_radiance = estimate_mis(rt, &r, &td.hit, depth);
	return (vec3_add(td.emission,
			vec3_mult_vecs(td.atten, td.indirect_radiance)));
}
