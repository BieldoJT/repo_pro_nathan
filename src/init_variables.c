/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_variables.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: natrodri <natrodri@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 16:50:38 by gda-conc          #+#    #+#             */
/*   Updated: 2025/09/26 17:16:33 by natrodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void    set_ambient(t_rt *rt, double ratio, t_vec3 color)
{
    rt->ambient.ratio = ratio;
    rt->ambient.color = color;
}

static void	create_world(t_rt *rt, t_scene *scene);

void add_sphere(t_prs_sphere *sph, t_rt *rt)
{
    while (sph)
    {
        rt->world[rt->world_size++] = sphere_create(
            vec3(sph->pos[0], sph->pos[1], sph->pos[2]),
            sph->radius,
            lambertian_create(vec3(
                sph->color[0] / 255.0,
                sph->color[1] / 255.0,
                sph->color[2] / 255.0))
        );
        sph = sph->next;
    }   
}

void add_plane(t_prs_plane *pl, t_rt *rt)
{
    while (pl)
    {
        rt->world[rt->world_size++] = plane_creat(
            vec3(pl->pos[0], pl->pos[1], pl->pos[2]),
            vec3(pl->orientation[0], pl->orientation[1], pl->orientation[2]),
            lambertian_create(vec3(
                pl->color[0] / 255.0,
                pl->color[1] / 255.0,
                pl->color[2] / 255.0))
        );
        pl = pl->next;
    }
}

void add_cylinder(t_prs_cylinder *cyl, t_rt *rt)
{
    double ra_and_he[2];

    while (cyl)
    {
        ra_and_he[0] = cyl->radius;
        ra_and_he[1] = cyl->height;
        rt->world[rt->world_size++] = cylinder_create(
            vec3(cyl->pos[0], cyl->pos[1], cyl->pos[2]),
            vec3(cyl->orientation[0], cyl->orientation[1], cyl->orientation[2]),
            ra_and_he,
            lambertian_create(vec3(
                cyl->color[0] / 255.0,
                cyl->color[1] / 255.0,
                cyl->color[2] / 255.0))
        );
        cyl = cyl->next;
    }
}

void add_lights(t_prs_light *lt, t_rt *rt)
{
    int count = 0;
    t_prs_light *tmp = lt;

    while (tmp)
    {
        count++;
        tmp = tmp->next;
    }
    rt->n_lights = count;
    rt->lights = malloc(sizeof(t_point_light *) * count);
    int i = 0;
    while (lt)
    {
        rt->lights[i++] = point_light_create(
    		vec3(lt->pos[0], lt->pos[1], lt->pos[2]),
    		vec3(
        		lt->color[0],
        		lt->color[1],
        		lt->color[2]
    		)
		);
        lt = lt->next;
    }
}

void	init_rt(t_rt *rt, t_scene *scene)
{
	double	aspect_ratio;

	aspect_ratio = 16.0 / 9.0;
	rt->mlx = NULL;
	rt->world_size = 0;
	rt->t_range = (t_interval){0, 0};
	rt->intensity = (t_interval){0, 0};
	interval_init(&rt->t_range, 0.001, INFINITY);
	interval_init(&rt->intensity, 0.000, 0.999);
	rt->image_width = 1000;
	rt->image_height = (int)(rt->image_width / aspect_ratio);
	if (rt->image_height < 1)
		rt->image_height = 1;
	rt->mlx = malloc(sizeof(t_mlx));
	init_mlx(rt);
	rt->camera = init_camera(&scene->camera, aspect_ratio, rt->image_width, rt->image_height);
    rt->camera->count_samples = 0;
    rt->camera->sample_per_pixel = 500;
    rt->camera->pixel_sample_scale = 1.0 / rt->camera->sample_per_pixel;
    rt->camera->max_depth = 20;
	set_ambient(rt, 0.8, vec3(1.0, 1.0, 1.0));
	create_world(rt, scene);
}

static void create_world(t_rt *rt, t_scene *scene)
{
    rt->world_size = 0;
    rt->world = malloc(sizeof(t_hittable *) * 20);
    if (!rt->world)
        return ;
    if (scene->spheres)
        add_sphere(scene->spheres, rt);
    if (scene->planes)
        add_plane(scene->planes, rt);
    if (scene->cylinders)
        add_cylinder(scene->cylinders, rt);
    if (scene->lights)
        add_lights(scene->lights, rt);
}

