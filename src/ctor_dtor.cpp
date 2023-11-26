#include "ctor_dtor.h"
#include "verror.h"
#include <stdlib.h>

struct tree_node *Node(void *val, type_t type)
{
  struct tree_node *node = (struct tree_node *)calloc(sizeof(tree_node), 1);
  if(!node)
  {
    VERROR_MEM;
    return NULL;
  }

  node->type = type;
  node->left = NULL;
  node->right = NULL;

  if(type == NOTHING) // maybe switch
  {
    node->digit = 0;
    return node;
  }

  else if(type == DIGIT)
  {
    node->digit = *(double *)val;
  }
  else if(type == VAR)
  {
    node->var = strndup((char *)val, strlen((char *)val));
  }
  else 
  {
    node->op = *(op_t *)val;
  }

  return node;
}

void Del_tree(struct tree_node *node)
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
  
  if(node->type == VAR)
  {
    free(node->var);
  }
  free(node);
}


