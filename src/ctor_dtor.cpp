#include "tree.h"
#include "ctor_dtor.h"
#include "verror.h"
#include <stdlib.h>

struct tree_node *Node(struct node_val val, struct tree_node *left, struct tree_node *right)
{
  struct tree_node *node = (struct tree_node *)calloc(sizeof(tree_node), 1);
  if(!node)
  {
    VERROR_MEM;
    return NULL;
  }

  node->left = left;
  node->right = right;

  node->val = val;
  if(val.type == VAR)
  {
    (node->val).var = strdup(val.var);
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
  
  if(node->val.type == VAR)
  {
    free(node->val.var);
  }
  free(node);
}


