/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gda-conc <gda-conc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 21:13:40 by gda-conc          #+#    #+#             */
/*   Updated: 2025/09/24 03:13:14 by gda-conc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static int	hit_anything(t_rt *rt, t_ray r, double tmax)
{
	t_hit_record	rec;
	t_interval		shadow_rng;

	interval_init(&shadow_rng, 0.001, tmax);
	return (hit_world(r, &rec, rt) != 0);
}

int	shadow_blocked(t_rt *rt, t_vec3 p, t_vec3 to_l, double dist_l)
{
	t_ray	r;

	r = ray(p, to_l);
	return (hit_anything(rt, r, dist_l - 1e-4));
}

t_vec3	point_light_diffuse(t_rt *rt, const t_hit_record *rec, int i)
{
	t_vec3	lo;
	t_vec3	nl;
	double	dist;
	double	n_dot_l;
	t_vec3	c;

	lo = vec3_sub(rt->lights[i]->position, rec->p);
	dist = vec3_length(lo);
	if (dist <= 0.0)
		return (vec3(0, 0, 0));
	nl = vec3_unit_vector(lo);
	if (shadow_blocked(rt, rec->p, nl, dist))
		return (vec3(0, 0, 0));
	n_dot_l = vec3_dot(rec->normal, nl);
	if (n_dot_l <= 0.0)
		return (vec3(0, 0, 0));
	/* 1/r^2 opcional: comente a linha abaixo se não quiser atenuação */
	c = vec3_div(rt->lights[i]->intensity, (dist * dist));
	/* sem 1/r^2 ficaria: c = rt->lights[i]->intensity; */
	return (vec3_mul(vec3_mult_vecs(rec->material->albedo, c), n_dot_l));
}

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
/*
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
*/

static t_vec3	ambient_term(t_rt *rt, const t_hit_record *rec)
{
	t_vec3	ia;      /* intensidade ambiente (cor * razão) */
	t_vec3	out;

	ia = vec3_mul(rt->ambient.color, rt->ambient.ratio);
	/* usa albedo do material para "tingir" a luz ambiente */
	out = vec3_mult_vecs(ia, rec->material->albedo);
	return (out);
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
	{t_vec3 dir = vec3_unit_vector(r.dir);
double t = 0.5 * (dir.y + 1.0);
return vec3_add( vec3_mul(vec3(1,1,1), (1.0 - t)),
                 vec3_mul(vec3(0.5,0.7,1.0), t) );}
		//return (vec3(0.0, 0.0, 0.0));
	td.emission = td.hit.material->color_emited;
	if (!td.hit.material->scatter(td.hit.material, &td.scatter_params))
		return (td.emission);
	if (depth <= 5 && rr_terminate(&td.atten))
		return (td.emission);
	t_vec3 amb = ambient_term(rt, &td.hit);
t_vec3 direct = vec3(0.0, 0.0, 0.0);
int li = 0;
while (li < rt->n_lights)
{
	direct = vec3_add(direct, point_light_diffuse(rt, &td.hit, li));
	li++;
}

/* ramo especular */
if (td.scatter_params.is_specular)
	return vec3_add(
		amb,
		vec3_add(
			td.emission,
			vec3_mult_vecs(td.atten, vec3_add(direct,
				ray_color(td.ray_next, rt, depth - 1))))
	);

/* ramo difuso (MIS indireto) */
return vec3_add(
	amb,
	vec3_add(
		td.emission,
		vec3_mult_vecs(td.atten, direct)
	)
);
}
