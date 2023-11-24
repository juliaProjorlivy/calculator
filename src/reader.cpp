#include "reader.h"
#include "tree.h"
#include "ctor_dtor.h"
#include "verror.h"

const int max_len = 10;

tree_node *reader(const char **line, variable **variables)
{
  if(**line == '(')
  {
    (*line)++;
    tree_node *node = Node(NULL, NOTHING);
    node->left = reader(line);
    
    int word_len = 0;
    double val = 0;
    if(sscanf(*line, "%lf%n", &val, &word_len) > 0)
    {
      node->digit = val;
      node->type = DIGIT;
      (*line) += word_len;
    }
    else
    {
      char op_val[max_len] = {};
      if(sscanf(*line, "%s%n", op_val, &word_len) < 0)
      {
        VERROR("troubles reading the line");
        return NULL;
      }
      if(!is_op(op_val))
      {
        
      }
      node->type = OP;
      node->op = strndup(op_val, word_len);
    }
  }
  if(**line == ')')
  {
    (*line)++;
    return node
  }
}


