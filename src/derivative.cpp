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
                struct tree_node *mul_r = Node(val, ln_L, c(node->right)); // произведение правой части узла на лог левой

                val.op = MUL;
                return Node(val, c(node), d(mul_r, var));
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
    if(op == SIN || op ==COS || op == LN || op == EXP)
    {
        return 1;
    }
    return 0;
}

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
                if(is_equal((node->left->val.val), 0))
                {
                    Del_tree(node);
                    return Node(val, NULL, NULL);
                }
            }
            if(node->right->val.type == DIGIT)
            {
                if(is_equal((node->right->val.val), 0))
                {
                    Del_tree(node);
                    return Node(val, NULL, NULL);
                }
            }
        }

        if(is_un(node->val.op))
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


