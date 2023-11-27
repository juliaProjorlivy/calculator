#ifndef TREE_H
#define TREE_H

typedef enum
{
  OP = 0,
  DIGIT = 1,
  VAR = 2,
  NOTHING = 3,
} type_t;

static const int max_len = 10;

static char op_names[][max_len] = {" ", "+", "-", "*", "/", "^", "sin", "cos", "ln", "exp"};
static int n_op = sizeof(op_names)/(sizeof(char) * max_len);
typedef enum
{
    NONE = 0,
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    POW = 5,
    SIN = 6,
    COS = 7,
    LN = 8,
    EXP = 9,
} op_t;

struct node_val
{
  type_t type;
  union
  {
      double val;
      op_t op;
      char *var;
  };
};


struct tree_node
{
  struct node_val val;
  
  struct tree_node *left;
  struct tree_node *right;
};



#endif

