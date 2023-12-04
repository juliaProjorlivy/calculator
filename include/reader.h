#ifndef READER_H
#define READER_H

struct tree_node *reader(char **line);

static const int max_len = 10;

static char op_names[][max_len] = {" ", "+", "-", "*", "/", "^", "sin", "cos", "ln", "exp"};

#endif