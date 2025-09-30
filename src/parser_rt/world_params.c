#include "parser.h"

void creat_ambient(char *str, t_scene *scene, char *line, int fd)
{
    char **tok;
    int i;

    tok = ft_split(str, ' ');
    i = 0;
    while (tok[i])
        i++;
    if (i != 3)
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(printf("argumentos errados pro ambient paizao\n"));
    }
    scene->ambient.intensity = ft_atof(tok[1]);
    if (!convert_color(tok[2], scene->ambient.color))
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(0);
    }
    free_split(tok);
}

void parse_camera(char *str, t_scene *scene, char *line, int fd)
{
    char **tok;
    int i;

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
        exit(printf("argumentos errados pra camera paizao\n"));
    }
    if (!convert_vec(tok[1], scene->camera.pos) || !convert_vec(tok[2], scene->camera.orientation))
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(0);
    }
    scene->camera.fov = ft_atof(tok[3]);
    free_split(tok);
}

void parse_light(char *str, t_scene *scene, char *line, int fd)
{
    char **tok;
    t_prs_light *light;
    int i;

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
        exit(printf("argumentos errados pra luz paizao\n"));
    }
    light = malloc(sizeof(t_prs_light));
    if (!light)
    {
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(printf("malloc falhou paizao\n"));
    }
    if (!convert_vec(tok[1], light->pos) || !convert_color(tok[3], light->color))
    {
        free(light);
        free_split(tok);
        all_free(scene);
        free(line);
        while ((line = get_next_line(fd)))
            free(line);
        exit(0);
    }
    light->intensity = ft_atof(tok[2]);
    light->next = scene->lights;
    scene->lights = light;
    free_split(tok);
}
