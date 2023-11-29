#include "tree.h"
#include "reader.h"
#include "ctor_dtor.h"
#include "derivative.h"
#include "verror.h"

static int n_op = sizeof(op_names)/(sizeof(char) * max_len);

static op_t is_op(char *op_val, int word_len)
{
    for(int i = 1; i < n_op; i++)
    {
        if(!strncmp(op_names[i], op_val, (size_t)word_len))
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
        struct node_val value = {.type = NOTHING, .val = 0};
        struct tree_node *node = Node(value, reader(line), NULL);

        int word_len = 0;
        double val = 0;
        if(sscanf(*line, "%lf%n", &val, &word_len) > 0)
        {
            node->val.val = val;
            node->val.type = DIGIT;
            (*line) += word_len; 
        }
        else if(**line == ')')
        {
            (*line)++;
            struct tree_node *new_node = c(node->left);
            Del_tree(node);
            return new_node;
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
                node->val.op = operation;
                node->val.type = OP;
                (*line) += word_len;
                *(bracket_ptr + word_len) = '(';
            }
            else
            {
                node->val.var = strndup(op_name, (size_t)word_len);
                node->val.type = VAR;
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
// // 

// struct tree_node *reader(char **line, int is_un_arg)
// {
//     if(**line == '(')
//     {
//         (*line)++;
//         struct node_val value = {.type = NOTHING, .val = 0};
//         struct tree_node *node = Node(value, reader(line), NULL);

//         int word_len = 0;
//         double val = 0;
//         if(sscanf(*line, "%lf%n", &val, &word_len) > 0)
//         {
//             node->val.val = val;
//             node->val.type = DIGIT;
//             (*line) += word_len; 
//         }
//         else
//         {
//             char op_name[max_len] = {};
//             char *bracket_ptr = strtok(*line, "()");
//             if(!bracket_ptr)
//             {
//                 VERROR("incorrect input");
//                 return NULL;
//             }
//             if(sscanf(*line, "%s%n", op_name, &word_len) < 0)
//             {
//                 VERROR_SSCANF(*line);
//                 return NULL;
//             }
//             op_t operation = is_op(op_name, word_len);
//             if(operation != NONE)
//             {
//                 node->val.op = operation;
//                 node->val.type = OP;
//                 (*line) += word_len;
//                 *(bracket_ptr + word_len) = '(';
//             }
//             else
//             {
//                 node->val.var = strndup(op_name, (size_t)word_len);
//                 node->val.type = VAR;
//                 (*line) += word_len;
//                 *(bracket_ptr + word_len) = ')';
//             }
//         }

//         node->right = reader(line);
//         if(**line != ')')
//         {
//             VERROR("incorrect input");
//             return NULL;
//         }
//         (*line)++;
//         return node;
//     }
//     return NULL;
// }

