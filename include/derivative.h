#ifndef FUNC_H
#define FUNC_H 

const double DEF_EPSILON = 1e-9;

int is_equal(double x, double y, double epsilon = DEF_EPSILON);

int is_un(op_t op);

struct tree_node *simplify(struct tree_node *node);

struct tree_node *d(struct tree_node *node, char *var);

struct tree_node *c(struct tree_node *node);

#endif
