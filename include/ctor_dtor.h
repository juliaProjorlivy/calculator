#ifndef CTOR_DTOR_H
#define CTOR_DTOR_H

#include "tree.h"

struct tree_node *Node(void *val, type_t type);

void Del_tree(struct tree_node *node);

#endif
