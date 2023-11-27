#ifndef FUNC_H
#define FUNC_H 

struct variables *Variables(int count, ...);

void Del_Variables(struct variables *vars);

int do_operation(op_t op, double *l_val, double *r_val, double *result);

double *find_value_of_variable(char *var, struct variables *vars);

int do_function(struct tree_node *node, struct variables *vars, double *result);

struct tree_node *d(struct tree_node *node, char *var);

struct tree_node *c(struct tree_node *node);

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


#endif
