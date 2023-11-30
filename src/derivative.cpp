#include "tree.h"
#include "ctor_dtor.h"
#include "derivative.h"
#include "variables.h"
#include "calculate_function.h"
#include "verror.h"
#include <math.h>


struct tree_node *c(struct tree_node *node)
{
    if(!node)
    {
        return NULL;
    }
    return Node(node->val, c(node->left), c(node->right));
}

// val passed as a general pointer is not a good idea
// in ctor pass a structure {type, union{val: double, char *, op_t}}
struct tree_node *d(struct tree_node *node, char *var)
{
    if(!node)
    {
        return NULL;
    }

    if(node->val.type == DIGIT)
    {
        struct node_val val = {.type = DIGIT, .val = 0};
        return Node(val, NULL, NULL);
    } 
    else if(node->val.type == VAR)
    {
        if(!strcmp(var, node->val.var))
        {
            struct node_val val = {.type = DIGIT, .val = 1};
            return Node(val, NULL, NULL);
        }
        struct node_val val = {.type = DIGIT, .val = 0}; // TODO: mathematics
        return Node(val, NULL, NULL);
    }
    else if(node->val.type == OP)
    {
        struct node_val val = {.type = OP};
        switch(node->val.op)
        {
            case ADD: [[fallthrough]] ;
            case SUB:
            {
                val.op = node->val.op;
                return Node(val, d(node->left, var), d(node->right, var));
            }
            case MUL:
            {
                val.op = MUL;
                struct tree_node *dleft = Node(val, d(node->left, var), c(node->right));
                struct tree_node *dright = Node(val, c(node->left), d(node->right, var));
                val.op = ADD;
                return Node(val, dleft, dright);
            }
            case DIV:
            {
                val.op = MUL;
                struct tree_node *dll = Node(val, d(node->left, var), c(node->right));
                struct tree_node *dlr = Node(val, c(node->left), d(node->right, var));
                struct tree_node *dr = Node(val, c(node->right), c(node->right));
                
                val.op = SUB;
                struct tree_node *dl = Node(val, dll, dlr);
        
                val.op = DIV;
                return Node(val, dl, dr);
            }
            case COS:
            {
                val.type = DIGIT;
                val.val = -1;
                struct tree_node *minus_one = Node(val, NULL, NULL);

                val.type = OP;
                val.op = SIN;
                struct tree_node *rl = Node(val, NULL, c(node->right));

                val.op = MUL;
                struct tree_node *r = Node(val, rl, d(node->right, var));

                return Node(val, minus_one, r);
            }
            case SIN:
            {
                val.op = COS;
                struct tree_node *l = Node(val, NULL, c(node->right));
                val.op = MUL;

                return Node(val, l, d(node->right, var));
            }
            case LN:
            {
                val.type = DIGIT;
                val.val = 1;
                struct tree_node *ll = Node(val, NULL, NULL);

                val.type = OP;
                val.op = DIV;
                struct tree_node *l = Node(val, ll, c(node->right));

                val.op = MUL;
                return Node(val, l, d(node->right, var));
            }
            case POW:
            {
                val.op = LN;
                struct tree_node *ln_L = Node(val, NULL, c(node->left)); // логарифм левой части дерева

                val.op = MUL;
                struct tree_node *rl = Node(val, d(ln_L, var), c(node->right));
                struct tree_node *rr = Node(val, ln_L, d(node->right, var));

                val.op = ADD;
                struct tree_node *r = Node(val, rl, rr);

                val.op = MUL;
                return Node(val, c(node), r);

            }
            case EXP:
            {
                val.op = MUL;
                return Node(val, c(node), d(node->right, var));
            }
            case NONE:
            {
                VERROR("unexpected operation NONE");
                break;
            }
            default:
            {
                VERROR("unexpected operation");
                break;
            }
        }

    }
    return NULL;
}

int is_un(op_t op)
{
    if(op == SIN || op == COS || op == LN || op == EXP)
    {
        return 1;
    }
    return 0;
}
