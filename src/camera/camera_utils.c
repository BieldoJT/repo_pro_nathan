/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bieldojt <bieldojt@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 17:40:16 by natrodri          #+#    #+#             */
/*   Updated: 2025/08/12 17:23:55 by bieldojt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../rt.h"

static double deg_to_rad(double degrees)
{
    return degrees * M_PI / 180.0;
}

t_camera *init_camera(t_prs_camera *prs_cam, double aspect_ratio, int image_width, int image_height)
{
    t_camera *cam;
    t_vec3 forward, right, up;

    if (!prs_cam)
        return NULL;

    cam = malloc(sizeof(t_camera));
    if (!cam)
        return NULL;

    cam->img_width = image_width;
    cam->img_height = image_height;
    cam->camera_center = vec3(prs_cam->pos[0], prs_cam->pos[1], prs_cam->pos[2]);
    cam->focal_length = 1.0;

    cam->viewport_height = 2.0 * tan(deg_to_rad(prs_cam->fov) / 2.0);
    cam->viewport_width = cam->viewport_height * aspect_ratio;

    forward = vec3_unit_vector(vec3(prs_cam->orientation[0], prs_cam->orientation[1], prs_cam->orientation[2]));
    right = vec3_unit_vector(vec3_cross((t_vec3){0,1,0}, forward));
    up = vec3_cross(forward, right);

    cam->viewport_u = vec3_mul(right, cam->viewport_width);
    cam->viewport_v = vec3_mul(up, cam->viewport_height);

    // Canto superior esquerdo do viewport
    cam->upper_left_corner = vec3_add(cam->camera_center, vec3_mul(forward, cam->focal_length));
    cam->upper_left_corner = vec3_sub(cam->upper_left_corner, vec3_mul(right, cam->viewport_width / 2.0));
    cam->upper_left_corner = vec3_add(cam->upper_left_corner, vec3_mul(up, cam->viewport_height / 2.0));

    cam->pixel_du_x = cam->viewport_width / (double)image_width;
    cam->pixel_dv_y = cam->viewport_height / (double)image_height;

    cam->sample_per_pixel = 600;
    cam->pixel_sample_scale = 1.0 / cam->sample_per_pixel;
    cam->max_depth = 50;
    cam->background_color = vec3(0.5, 0.7, 1.0);

    cam->count_samples = 0;

    return cam;
}

static t_vec3 sample_stratified(int s, int n)
{
    int sample_x = s % n;
    int sample_y = s / n;
    double dx = ((double)sample_x + random_double()) / (double)n - 0.5;
    double dy = ((double)sample_y + random_double()) / (double)n - 0.5;
    return (t_vec3){dx, dy, 0.0};
}

// Constrói um raio passando pelo pixel (i,j) com jitter
t_ray get_ray(const t_camera *cam, int i, int j, int *sample_index)
{
    t_vec3 offset = sample_stratified(sample_index[0], sample_index[1]);
    double u = ((double)i + 0.5 + offset.x) / (cam->img_width - 1);
    double v = ((double)j + 0.5 + offset.y) / (cam->img_height - 1);

    t_vec3 pixel_sample = vec3_add(cam->upper_left_corner,
                                   vec3_add(vec3_mul(cam->viewport_u, u),
                                            vec3_mul(cam->viewport_v, -v)));
    return ray(cam->camera_center, vec3_sub(pixel_sample, cam->camera_center));
}

