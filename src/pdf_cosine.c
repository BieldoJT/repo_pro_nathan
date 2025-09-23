/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pdf_cosine.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gda-conc <gda-conc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 10:47:50 by gda-conc          #+#    #+#             */
/*   Updated: 2025/09/22 11:03:48 by gda-conc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static double	value_c(t_pdf *p, t_vec3 d)
{
	double	c;

	(void)p;
	d = vec3_unit_vector(d);
	c = d.z;
	if (c <= 0.0)
		return (0.0);
	return (c / PI);
}

static t_vec3	generate_c(t_pdf *pdf)
{
	double	ray1;
	double	ray2;
	double	phi;
	double	x_y_z[3];
	t_pdfc	*c;

	c = (t_pdfc *)pdf->data;
	ray1 = random_double();
	ray2 = random_double();
	x_y_z[2] = sqrt(1.0 - ray2);
	phi = 2.0 * PI * ray1;
	x_y_z[0] = cos(phi) * sqrt(ray2);
	x_y_z[1] = sin(phi) * sqrt(ray2);
	return (onb_local(&c->basis, x_y_z[0], x_y_z[1], x_y_z[2]));
}

t_pdf	pdf_cosine_make(t_vec3 normal)
{
	t_pdf	pdf;
	t_pdfc	*cosine;

	cosine = (t_pdfc *)malloc(sizeof(t_pdfc));
	if (!cosine)
		return ((t_pdf){0});
	onb_build(&cosine->basis, normal);
	pdf.data = cosine;
	pdf.value = &value_c;
	pdf.generate = &generate_c;
	return (pdf);
}
