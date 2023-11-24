#include "ctor_dtor.h"
#include "verror.h"
#include <stdlib.h>

int Node(void *val, type_t type)
{
  tree_node *node = (tree_node *)calloc(sizeof(tree_node), 1)
  if(!node)
  {
    VERROR_MEM;
    return 1;
  }

  node->type = type;
  node->left = NULL;
  node->right = NULL;

  if(type == NOTHING)
  {
    node->op = NULL;
    return 0;
  }

  else if(type == DIGIT)
  {
    node->digit = *(double *)val;
  }
  else if(type == VAR)
  {
    node->var = val;
  }
  else 
  {
    node->symb = strndub((char *)val, strlen((char *)val))  
  }

  return 0;
}

void Del_tree(tree_node *node)
{
  if(!node)
  {
    return;
  }
  if(node->left)
  {
    Del_tree(node->left);
  }
  if(node->right)
  {
    Del_tree(node->right);
  }
  
  if(node->type == OP)
  {
    free(node->symb);
  }
  free(node);
}


