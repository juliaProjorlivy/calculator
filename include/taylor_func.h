#ifndef TAYLOR_FUNC_H
#define TAYLOR_FUNC_H

struct tree_node *Taylor_expansion(struct tree_node *node, struct variable *var, int degree);

struct tree_node *Taylor_term(struct tree_node *node, struct variables *vars, int cur_degree, int degree);

#endif
