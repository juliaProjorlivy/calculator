#include "ctor_dtor.h"
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
        if(!strncmp(var, node->val.var, strlen(var)))
        {
            struct node_val val = {.type = DIGIT, .val = 1};
            return Node(val, NULL, NULL);
        }
        struct node_val val = {.type = VAR, .var = strdup(node->val.var)};
        return Node(val, NULL, NULL);
    }
    else if(node->val.type == OP)
    {
        struct node_val val = {.type = OP, .op = ADD};
        switch(node->val.op)
        {
            case ADD:
            {
                return Node(val, d(node->left, var), d(node->right, var));
            }
            case SUB:
            {
                val.op = SUB;
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
                struct tree_node *rl = Node(val, c(node->right), NULL);

                val.op = MUL;
                struct tree_node *r = Node(val, rl, d(node->right, var));

                return Node(val, minus_one, r);
            }
        }

    }
    return NULL;
}


