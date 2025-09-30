#include "parser.h"

void parse_sphere(char *str, t_scene *scene, char *line, int fd)
{
    char      **tok;
    t_prs_sphere  *sp;
    int       i;

    tok = ft_split(str, ' ');
    i = 0;
    while (tok[i])
        i++;
    if (i != 4)
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(printf("argumentos errados pra esfera paizao\n"));
    }
    sp = malloc(sizeof(t_prs_sphere));
    if (!sp)
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(printf("malloc falhou paizao\n"));
    }
    if (!convert_vec(tok[1], sp->pos) || !convert_color(tok[3], sp->color))
    {
        free(sp);
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(0);
    }
    sp->radius = ft_atof(tok[2]) / 2.0;
    sp->next = scene->spheres;
    scene->spheres = sp;
    free_split(tok);
}

void parse_plane(char *str, t_scene *scene, char *line, int fd)
{
    char     **tok;
    t_prs_plane  *pl;
    int       i;

    tok = ft_split(str, ' ');
    i = 0;
    while (tok[i])
        i++;
    if (i != 4)
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(printf("argumentos errados pro plano paizao\n"));
    }
    pl = malloc(sizeof(t_prs_plane));
    if (!pl)
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(printf("malloc falhou paizao\n"));
    }
    if (!convert_vec(tok[1], pl->pos) || !convert_vec(tok[2], pl->orientation) || !convert_color(tok[3], pl->color))
    {
        free(pl);
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(0);
    }
    pl->next = scene->planes;
    scene->planes = pl;
    free_split(tok);
}

void parse_cylinder(char *str, t_scene *scene, char *line, int fd)
{
    char        **tok;
    t_prs_cylinder  *cy;
    int         i;

    tok = ft_split(str, ' ');
    i = 0;
    while (tok[i])
        i++;
    if (i != 6)
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(printf("argumentos errados pro cilindro paizao\n"));
    }
    cy = malloc(sizeof(t_prs_cylinder));
    if (!cy)
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(printf("malloc falhou paizao\n"));
    }
    if (!convert_vec(tok[1], cy->pos) || !convert_vec(tok[2], cy->orientation) || !convert_color(tok[5], cy->color))
    {
        free(cy);
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(0);
    }
    cy->radius = ft_atof(tok[3]) / 2.0;
    cy->height = ft_atof(tok[4]);
    cy->next = scene->cylinders;
    scene->cylinders = cy;
    free_split(tok);
}
