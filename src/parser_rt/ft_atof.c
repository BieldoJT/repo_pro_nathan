#include "parser.h"

double  ft_atof(char *str)
{
    int     i;
    int     sig;
    double  int_part;
    double  divisor;
    double  frac_part;

    i = 0;
    sig = 1;
    int_part = 0;
    frac_part = 0;
    divisor = 1;
    while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
        i++;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sig = -1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9')
        int_part = int_part * 10 + (str[i++] - '0');
    if (str[i] == '.')
    {
        i++;
        while (str[i] >= '0' && str[i] <= '9')
        {
            frac_part = frac_part * 10 + (str[i++] - '0');
            divisor *= 10;

        }
    }
    return (sig * (int_part + frac_part / divisor));
}