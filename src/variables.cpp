#include "variables.h"
#include "verror.h"
#include <math.h>
#include <stdarg.h>

                    //    number of elements in arr of structs multiplied by 2
struct variables *Variables(int count, ...)
{
    struct variables *vars = (struct variables *)calloc(sizeof(variables), 1);
    if(!vars)
    {
        VERROR_MEM;
        return NULL;
    }
    vars->count = count / 2;
    struct variable *var = (struct variable *)calloc(sizeof(variable), (size_t)count);
    if(!var)
    {
        VERROR_MEM;
        free(vars);
        return NULL;
    }
    vars->var_arr = var;

    va_list names_vals;
    va_start(names_vals, count);
    for(int i = 0; i < count / 2; i++)
    {
        var[i].name = strdup(va_arg(names_vals, char *));
        var[i].val = va_arg(names_vals, double);
    }
    va_end(names_vals);
    return vars;
}

void Del_Variables(struct variables *vars)
{
    for(int i = 0; i < vars->count; i++)
    {
        free((vars->var_arr)[i].name);
    }
    free(vars->var_arr);
    free(vars);
}

double *find_value_of_variable(char *var, struct variables *vars)
{
    for(int i_var = 0; i_var < vars->count; i_var++)
    {
        if(!strcmp((vars->var_arr)[i_var].name, var)) // TODO: strcmp
        {
            return &((vars->var_arr)[i_var].val);
        }
    }
    return NULL;
}
