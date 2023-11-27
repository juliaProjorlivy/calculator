#ifndef CTOR_DTOR_H
#define CTOR_DTOR_H

#include "tree.h"

struct tree_node *Node(struct node_val val, struct tree_node *left, struct tree_node *right);

void Del_tree(struct tree_node *node);

#endif
