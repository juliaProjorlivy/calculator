#include "tree.h"
#include "reader.h"
#include "ctor_dtor.h"
#include "verror.h"

static op_t is_op(char *op_val, int word_len)
{
    for(int i = 1; i < n_op; i++)
    {
        if(!strncmp(op_names[i], op_val, word_len))
        {
            return (op_t)i;
        }
    }
    return NONE;
}

struct tree_node *reader(char **line)
{
    if(**line == '(')
    {
        (*line)++;
        struct tree_node *node = Node(NULL, NOTHING);
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
            char op_name[max_len] = {};
            char *bracket_ptr = strtok(*line, "()");
            if(!bracket_ptr)
            {
                VERROR("incorrect input");
                return NULL;
            }
            if(sscanf(*line, "%s%n", op_name, &word_len) < 0)
            {
                VERROR_SSCANF(*line);
                return NULL;
            }
            op_t operation = is_op(op_name, word_len);
            if(operation != NONE)
            {
                node->op = operation;
                node->type = OP;
                (*line) += word_len;
                *(bracket_ptr + word_len) = '(';
            }
            else
            {
                node->var = strndup(op_name, word_len);
                node->type = VAR;
                (*line) += word_len;
                *(bracket_ptr + word_len) = ')';
            }
        }
        
        node->right = reader(line);
        if(**line != ')')
        {
            VERROR("incorrect input");
            return NULL;
        }
        (*line)++;
        return node;
    }
    return NULL;
}

