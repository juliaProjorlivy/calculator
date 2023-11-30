#include "tree.h"
#include "simplifier.h"
#include "ctor_dtor.h"
#include "calculate_function.h"
#include "derivative.h"
#include "variables.h"
#include "verror.h"
#include <math.h>

int is_equal(double x, double y, double epsilon)
{
    return (fabs (x - y) < epsilon);
}

struct tree_node *simplify(struct tree_node *node)
{
    if(!node)
    {
        return NULL;
    }
    node->left = simplify(node->left);
    node->right = simplify(node->right);

    struct variables vars = {};
    double new_val = 0;
    struct node_val val = {.type = DIGIT, .val = new_val};

    if(node->val.type == OP)
    {
        if(node->val.op == MUL)
        {
            if(node->left->val.type == DIGIT)
            {
                if(is_equal(node->left->val.val, 0))
                {
                    Del_tree(node);
                    return Node(val, NULL, NULL);
                }
                else if(is_equal(node->left->val.val, 1))
                {
                    Del_tree(node->left);
                    struct tree_node *ret_node = node->right;
                    free(node);
                    return ret_node;
                }
            }
            if(node->right->val.type == DIGIT)
            {
                if(is_equal((node->right->val.val), 0))
                {
                    Del_tree(node);
                    return Node(val, NULL, NULL);
                }
                else if(is_equal(node->right->val.val, 1))
                {
                    Del_tree(node->right);
                    struct tree_node *ret_node = node->left;
                    free(node);
                    return ret_node;
                }
            }
        }

        else if(node->val.op == ADD)
        {
            if(node->left->val.type == DIGIT && node->left->val.var == 0)
            {
                struct tree_node *ret_node = node->right;
                Del_tree(node->left);
                free(node);
                return ret_node;
            }
            if(node->right->val.type == DIGIT && node->right->val.var == 0)
            {
                struct tree_node *ret_node = node->left;
                Del_tree(node->right);
                free(node);
                return ret_node;
            }
        }

        else if(is_un(node->val.op))
        {
            if(node->right->val.type == DIGIT && !node->left)
            {
                if(do_function(node, &vars, &new_val)) 
                {
                    VERROR("toubles counting the function");
                    return NULL;
                }
                Del_tree(node);
                val.val = new_val;
                return Node(val, NULL, NULL);
            }
        }
        else if(node->right->val.type == DIGIT && node->left->val.type == DIGIT)
        {
            if(do_function(node, &vars, &new_val))
            {
                VERROR("troubles counting the function");
                return NULL;
            }
            Del_tree(node);
            val.val = new_val;
            return Node(val, NULL, NULL);
        }
    }

    return node;
}


