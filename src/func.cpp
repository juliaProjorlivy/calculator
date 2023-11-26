#include "tree.h"
#include "func.h"
#include "verror.h"
#include <cmath>
#include <math.h>
#include <stdarg.h>

//                        number of elements in arr of structs multiplied by 2
struct variables *Variables(int count, ...)
{
    struct variables *vars = (struct variables *)calloc(sizeof(variables), 1);
    if(!vars)
    {
        VERROR_MEM;
        return NULL;
    }
    vars->count = count / 2;
    struct variable *var = (struct variable *)calloc(sizeof(variable), count);
    if(!var)
    {
        VERROR_MEM;
        free(vars);
        return NULL;
    }
    vars->var_arr = var;

    va_list names_vals;
    va_start(names_vals, count);
    for(int i = 0; i < count / 2; i++)
    {
        var[i].name = strdup(va_arg(names_vals, char *));
        // var[i].val = va_arg(names_vals, double);
        var[i].val = 100;
    }
    va_end(names_vals);
    return vars;
}

void Del_Variables(struct variables *vars)
{
    for(int i = 0; i < vars->count; i++)
    {
        free((vars->var_arr)[i].name);
    }
    free(vars->var_arr);
    free(vars);
}

int do_operation(op_t op, double *l_val, double *r_val, double *result)
{
    switch(op)
    {
        case ADD:
        {
            // if(!l_val || !r_val)
            // {
            //     VERROR("incorrect input");
            //     return 1;
            // }
            *result = *l_val + *r_val;
            return 0;
        }
        case SUB:
        {
            if(!l_val || !r_val)
            {
                VERROR("incorrect input");
                return 1;
            }
            *result = *l_val - *r_val;
            return 0;
        }
        case MUL:
        {
            if(!l_val || !r_val)
            {
                VERROR("incorrect input");
                return 1;
            }
            *result = (*l_val) * (*r_val);
            return 0;
        }
        case DIV:
        {
            if(!l_val || !r_val)
            {
                VERROR("incorrect input");
                return 1;
            }
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
            if(!l_val || !r_val)
            {
                VERROR("incorrect input");
                return 1;
            }
            *result = pow((*l_val), (*r_val));
            return 0;
        }
        case SIN:
        {
            // if(l_val || !r_val)
            // {
            //     VERROR("incorrect input");
            //     return 1;
            // }
            *result = sin((*r_val));
            return 0;
        }
        case COS:
        {
            // if(l_val || !r_val)
            // {
            //     VERROR("incorrect input");
            //     return 1;
            // }
            *result = cos((*r_val));
            return 0;
        }
        case LN:
        {
            // if(l_val || !r_val)
            // {
            //     VERROR("incorrect input");
            //     return 1;
            // }
            *result = log((*r_val));
            return 0;
        }
        case EXP:
        {
            // if(l_val || !r_val)
            // {
            //     VERROR("incorrect input");
            //     return 1;
            // }
            *result = exp((*r_val));
            return 0;
        }
        default:
            VERROR("unexpected operation");
            break;
    }
    
    return 0;
}

double *find_value_of_variable(char *var, struct variables *vars)
{
    for(int i_var = 0; i_var < vars->count; i_var++)
    {
        if(!strncmp((vars->var_arr)[i_var].name, var, strlen(var)))
        {
            return &((vars->var_arr)[i_var].val);
        }
    }
    return NULL;
}

// double do_function(struct tree_node *node, struct variables *vars, double *result)
// {
//     if(!node)
//     {
//         return 0;
//     }
//     // double left = 0;
//     // double right = 0;
//     double l_val = do_function(node->left, vars, result);
//     // if(l_val)
//     // {
//     //     left = *l_val;
//     // }
//     double r_val = do_function(node->right, vars, result);
//     // if(r_val)
//     // {
//     //     right = *r_val;
//     // }
//     switch(node->type)
//     {
//         case OP:
//         {
//             if(do_operation(node->op, &l_val, &r_val, result))
//             {
//                 VERROR("unexpected operation");
//                 return 0;
//             }
//             break;
//         }
//         case DIGIT:
//         {
//             *result = node->digit;
//             break;
//         }
//         case VAR:
//         {
//             *result = *find_value_of_variable(node->var, vars);
//             if(!result) 
//             {
//                 VERROR("Troubles finding variable");
//                 return 0;
//             }
//             break;
//         }
//         case NOTHING:
//         {
//             VERROR("type NOTHING should not be in the tree");
//             return 0;
//         }
//         default:
//         {
//             VERROR("unexpected type in function");
//             return 0;
//         }
    
//     }
//     return *result;

// }

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
    
    switch(node->type)
    {
        case OP:
        {
            if(do_operation(node->op, &l_val, &r_val, result))
            {
                VERROR("unexpected operation");
                return 1;
            }
            break;
        }
        case DIGIT:
        {
            *result = node->digit;
            break;
        }
        case VAR:
        {
            *result = *find_value_of_variable(node->var, vars);
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

