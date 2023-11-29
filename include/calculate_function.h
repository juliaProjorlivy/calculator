#ifndef CALCULATE_FUNCTION_H
#define CALCULATE_FUNCTION_H

int do_operation(op_t op, double *l_val, double *r_val, double *result);

int do_function(struct tree_node *node, struct variables *vars, double *result);

#endif