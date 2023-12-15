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

const int double_size = 12;
const int operation_size = 3;

int node_size(struct tree_node *node)
{
    if(!node)
    {
        return 0;
    }

    int lsize = node_size(node->left);
    int rsize = node_size(node->right);
    int size = lsize + rsize;
    if(node->val.type == DIGIT)
    {
        size += double_size;
    }
    else if(node->val.type == OP)
    {
        size += operation_size;
    }
    else
    {
        size += strlen(node->val.var);
    }
    return size;
}


