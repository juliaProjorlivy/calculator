#ifndef DUMP_H
#define DUMP_H

#include <stdio.h>

void print_in_node(struct tree_node *node, char **line, char **const_line);

void node_dump(struct tree_node *node, FILE *file);

int tree_dump(struct tree_node *node, const char *filename);

#endif
