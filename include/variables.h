#ifndef VARIABLES_H
#define VARIABLES_H

struct variable
{
    char *name;
    double val;
};
struct variables
{
    int count;
    struct variable *var_arr;
};

struct variables *Variables(int count, ...);

void Del_Variables(struct variables *vars);

double *find_value_of_variable(char *var, struct variables *vars);

#endif