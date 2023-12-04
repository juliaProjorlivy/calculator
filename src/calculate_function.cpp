#include "tree.h"
#include "calculate_function.h"
#include "verror.h"
#include "math.h"
#include "variables.h"

int do_operation(op_t op, double *l_val, double *r_val, double *result)
{
    switch(op)
    {
        case ADD:
        {
            *result = *l_val + *r_val;
            return 0;
        }
        case SUB:
        {
            *result = *l_val - *r_val;
            return 0;
        }
        case MUL:
        {
            *result = (*l_val) * (*r_val);
            return 0;
        }
        case DIV:
        {
            if(r_val == 0)
            {
                VERROR("dvivision by zero");
                return 1;
            }
            *result = *l_val / *r_val;
            return 0;
        }
        case POW:
        {
            *result = pow((*l_val), (*r_val));
            return 0;
        }
        case SIN:
        {
            *result = sin((*r_val));
            return 0;
        }
        case COS:
        {
            *result = cos((*r_val));
            return 0;
        }
        case LN:
        {
            *result = log((*r_val));
            return 0;
        }
        case EXP:
        {
            *result = exp((*r_val));
            return 0;
        }
        case NONE:
        {
            VERROR("unexpected operation");
            break;
        }
        default:
            VERROR("unexpected operation");
            break;
    }
    
    return 0;
}


int do_function(struct tree_node *node, struct variables *vars, double *result)
{
    if(!node)
    {
        return 0;
    }

    if(do_function(node->left, vars, result))
    {
        return 1;
    }
    double l_val = *result;
    if(do_function(node->right, vars, result))
    {
        return 1;
    }
    double r_val = *result;
    
    switch(node->val.type)
    {
        case OP:
        {
            if(do_operation(node->val.op, &l_val, &r_val, result))
            {
                VERROR("unexpected operation");
                return 1;
            }
            break;
        }
        case DIGIT:
        {
            *result = node->val.val;
            break;
        }
        case VAR:
        {
            *result = *find_value_of_variable(node->val.var, vars);
            if(!result) 
            {
                VERROR("Troubles finding variable");
                return 1;
            }
            break;
        }
        case NOTHING:
        {
            VERROR("type NOTHING should not be in the tree");
            return 1;
        }
        default:
        {
            VERROR("unexpected type in function");
            return 1;
        }
    
    }
    return 0;
}
