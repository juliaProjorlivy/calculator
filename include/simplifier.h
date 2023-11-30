#ifndef SIMPLIFIER_H
#define SIMPLIFIER_H

const double DEF_EPSILON = 1e-9;

int is_equal(double x, double y, double epsilon = DEF_EPSILON);

struct tree_node *simplify(struct tree_node *node);

#endif