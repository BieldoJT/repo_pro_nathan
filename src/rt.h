/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bieldojt <bieldojt@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 17:17:55 by gda-conc          #+#    #+#             */
/*   Updated: 2025/09/17 10:58:38 by bieldojt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_H
# define RT_H

/*
** =============================================================================
** INCLUDES
** =============================================================================
*/

# include "../lib/libft.h"
# include "../minilibx-linux/mlx.h"
# include "vec3/vec3.h"
# include "ray.h"
# include "interval/interval.h"
# include <X11/X.h>
# include <X11/keysym.h>
# include <fcntl.h>
# include <float.h>
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <time.h>

/*
** =============================================================================
** MACROS
** =============================================================================
*/

# define ESC 65307
# define TRUE 1
# define FALSE 0
# define PI 3.1415926535897932385

/*
** =============================================================================
** FORWARD DECLARATIONS & ENUMS
** =============================================================================
*/

typedef struct s_hit_record		t_hit_record;
typedef struct s_material		t_material;
typedef struct s_scatter_params	t_scatter_params;
typedef int						(*t_scatter_fn)(const struct s_material *mat, \
									t_scatter_params *params);
typedef enum e_hittable_type
{
	SPHERE
}								t_hittable_type;

/*
** =============================================================================
** STRUCTURES DEFINITIONS
** =============================================================================
*/

//------------------------------------------------------------------------------
//|                                  MATERIAL                                  |
//------------------------------------------------------------------------------

typedef struct s_scatter_params
{
	const t_ray					*r_in;
	const t_hit_record			*rec;
	t_vec3						*attenuation;
	t_ray						*scattered;
	int							is_specular; //pro metal e o dialectrico
	double						pdf; //pro difuso
}								t_scatter_params;

typedef struct s_material
{
	t_scatter_fn				scatter;
	double						(*scattering_pdf)(const struct s_material *mat, \
									const t_ray *r_in, \
									const t_hit_record *rec, \
									t_vec3 *scattered);
	t_vec3						albedo;
	double						fuzz;
	double						refractive_index;
	t_vec3						color_emited;
}								t_material;

//------------------------------------------------------------------------------
//|                                 LIGHT                                      |
//------------------------------------------------------------------------------

typedef struct s_point_light
{
	t_vec3	position;
	t_vec3	intensity;
}	t_point_light;

//------------------------------------------------------------------------------
//|                            HITTABLES (OBJETOS)                             |
//------------------------------------------------------------------------------

typedef struct s_hit_record
{
	double		t;
	t_vec3		p;
	t_vec3		normal;
	t_material	*material;
	int			front_face;
	double		u;
	double		v;
}	t_hit_record;

typedef struct s_hittable
{
	void		*obj;
	int			(*hit)(void *object, t_ray r, t_interval, t_hit_record *rec);
}	t_hittable;

typedef struct s_sphere
{
	t_vec3		center;
	double		radius;
	t_material	*material;
}	t_sphere;

typedef struct s_equation
{
	t_vec3	oc;
	double	a;
	double	half_b;
	double	c;
	double	disc;
}	t_equation;

//------------------------------------------------------------------------------
//|                                   CÂMERA                                   |
//------------------------------------------------------------------------------

typedef struct s_camera
{
	t_vec3						camera_center;
	double						focal_length;
	double						viewport_height;
	double						viewport_width;
	t_vec3						viewport_u;
	t_vec3						viewport_v;
	t_vec3						upper_left_corner;
	double						pixel_du_x;
	double						pixel_dv_y;
	int							count_samples;
	int							sample_per_pixel;
	double						pixel_sample_scale;
	int							max_depth;
	t_vec3						background_color;
}								t_camera;

//------------------------------------------------------------------------------
//|                           RENDER & JANELA                        |
//------------------------------------------------------------------------------


typedef struct s_mlx
{
	void						*mlx_ptr;
	void						*win_ptr;
	void						*img;
	char						*addr;
	int							bits_per_pixel;
	int							line_length;
	int							endian;
}								t_mlx;

typedef struct s_rt
{
	t_mlx						*mlx;
	t_camera					*camera;
	int							image_width;
	int							image_height;
	int							world_size;
	t_hittable					**world;
	t_interval					t_range;
	t_interval					intensity;
	int							n_lights;
	t_point_light				**lights;
}								t_rt;

/*
** =============================================================================
** FUNCTION PROTOTYPES
** =============================================================================
*/

//------------------------------------------------------------------------------
//|                                  main.c                                    |
//------------------------------------------------------------------------------
void							init_rt(t_rt *rt);

//------------------------------------------------------------------------------
//|                                 render.c                                   |
//------------------------------------------------------------------------------
//void							render_rt(t_rt *rt);
void	render_rt(t_rt *rt); // no anti-aliasing
t_vec3							ray_color(t_ray r, t_rt *rt, int depth);

//------------------------------------------------------------------------------
//|                                  mlx.c                                     |
//------------------------------------------------------------------------------
void							init_mlx(t_rt *rt);
void							my_mlx_pixel_put(t_mlx *mlx, int x, int y, \
									int color);
int								destroy(t_mlx *mlx);
int								destroy_in_esc(int keycode, t_mlx *mlx);

//------------------------------------------------------------------------------
//|                                 camera.c                                   |
//------------------------------------------------------------------------------
t_camera						*init_camera(double aspect_ratio, \
									int image_width, int image_height);
void							destroy_camera(t_camera *camera);
t_vec3							get_pixel_center(t_camera *camera, int i, int j,
									t_vec3 pixel00);
t_ray							get_ray(const t_camera *cam, int i, int j,
									int *sample_index);
t_vec3							get_pixel00(t_camera *camera);

//------------------------------------------------------------------------------
//|                                hittable.c                                  |
//------------------------------------------------------------------------------
int								hit_world(t_ray r, t_hit_record *rec, t_rt *rt);
void							set_face_normal(t_hit_record *rec, t_ray r, \
									t_vec3 outward_normal);

//------------------------------------------------------------------------------
//|                                   onb.c                                    |
//------------------------------------------------------------------------------

typedef struct s_onb
{
	t_vec3	u;
	t_vec3	v;
	t_vec3	w;
}	t_onb;


/* Constrói ONB com w alinhado à normal n (n deve ser normalizada). */
void	onb_build(t_onb *b, t_vec3 n);

/* Converte coordenadas locais (a,b,c) para o espaço do mundo usando a ONB. */
t_vec3	onb_local(const t_onb *b, double a, double b2, double c);


//------------------------------------------------------------------------------
//|                                   pdf.c                                    |
//------------------------------------------------------------------------------

typedef struct s_pdf t_pdf;
typedef struct s_pdf
{
	double	(*value)(t_pdf *p, t_vec3 d);
	t_vec3	(*generate)(t_pdf *p);
	void	*data;
}	t_pdf;

typedef struct s_pdfc
{
	t_onb	basis;
}	t_pdfc;

typedef struct s_pdfm
{
	t_pdf	a;
	t_pdf	b;
}	t_pdfm;

/* PDF cosseno para difusos (usará ONB alinhada à normal). */
t_pdf	pdf_cosine_make(t_vec3 normal);

/* PDF para amostrar uma ESFERA EMISSIVA vista a partir de 'origin'. */
/* 'sphere' é um ponteiro para o seu t_sphere (void* para não vazar tipos). */
t_pdf	pdf_light_sphere_make(void *sphere, t_vec3 origin);

/* Mistura 50/50 de dois PDFs (cosine + light, p.ex.). */
t_pdf	pdf_mix_make(t_pdf a, t_pdf b);

//------------------------------------------------------------------------------
//|                                 sphere.c                                   |
//------------------------------------------------------------------------------
t_hittable						*sphere_create(t_vec3 center, double radius, \
									t_material *material);
void							sphere_destroy(t_hittable *hittable);
int								sphere_hit(void *object, t_ray r, t_interval t,
									t_hit_record *rec);
double							hit_sphere(t_vec3 center, double radius, \
									t_ray r);

/*
** Densidade do PDF (pelo sólido ângulo) de lançar uma direção 'dir'
** da origem 'origin' que acerta a esfera 'sp'. Retorna 0.0 se não visível.
** sp deve ser (t_sphere*), mas a assinatura fica genérica (void*) para pdf.h.
*/
double	sphere_pdf_value_from(void *sp, t_vec3 origin, t_vec3 dir);
/*
** Gera uma direção aleatória (no MUNDO) que aponta para a esfera 'sp'
** vista da origem 'origin', restrita ao cone que cobre a esfera.
*/
t_vec3	sphere_random_dir_to(void *sp, t_vec3 origin);

//------------------------------------------------------------------------------
//|                                material.c                                  |
//------------------------------------------------------------------------------

//---------------------------------LAMBERTIAN---------------------------------//
t_material						*lambertian_create(t_vec3 albedo);
double	lambertian_scattering_pdf(const t_material *mat, const t_ray *r_in,
			const t_hit_record *rec, t_vec3 *scattered);
t_vec3	lambertian_emitted(const t_material *mat, const t_hit_record *rec, double u, double v, t_vec3 p);
void	material_set_lambertian(t_material *mat, t_vec3 albedo);

//------------------------------------METAL------------------------------------//
t_material						*metal_create(t_vec3 albedo, double fuzz);
//double	metal_scattering_pdf(const t_material *mat, const t_ray *r_in, const t_hit_record *rec, const t_ray *scattered);
//t_vec3	metal_emitted(const t_material *mat, const t_hit_record *rec, double u, double v, t_vec3 p);
void	material_set_metal(t_material *mat, t_vec3 albedo, double fuzz);

//---------------------------------DIELECTRIC---------------------------------//
t_material						*dielectric_create(double refractive_index);
//double	dielectric_scattering_pdf(const t_material *mat, const t_ray *r_in, const t_hit_record *rec, const t_ray *scattered);
//t_vec3	dielectric_emitted(const t_material *mat, const t_hit_record *rec, double u, double v, t_vec3 p);
void	material_set_dielectric(t_material *mat, double refractive_index);

//--------------------------------DIFFUSE LIGHT-------------------------------//
t_material						*diffuse_light_create(t_vec3 albedo);
//double	diffuse_light_scattering_pdf(const t_material *mat, const t_ray *r_in, const t_hit_record *rec, const t_ray *scattered);
//t_vec3	diffuse_light_emitted(const t_material *mat, const t_hit_record *rec, double u, double v, t_vec3 p);
//void	material_set_diffuse_light(t_material *mat, t_vec3 emit_color);

void							material_destroy(t_material *m);

//------------------------------------------------------------------------------
//|                                 lights.c                                    |
//------------------------------------------------------------------------------

t_point_light	*point_light_create(t_vec3 position, t_vec3 intensity);

//------------------------------------------------------------------------------
//|                                 utils.c                                    |
//------------------------------------------------------------------------------
double							degree_to_radian(double degree);
double							random_double(void);
double							random_double_range(double min, double max);
int								rgb_to_int(int r, int g, int b);
t_vec3							sample_square(void);

#endif
