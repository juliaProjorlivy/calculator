#ifndef TREE_H
#define TREE_H

typedef struct
{
  union
  {
    double digit;
    char *op;
    char var;
  };

  Node *left;
  Node *right;
} Node;

typedef enum
{
  OP = 0,
  DIGIT = 1,
  VAR = 2,
  NOTHING = 3,
} type_t;

#endif

