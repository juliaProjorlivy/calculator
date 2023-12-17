#include <stdio.h>
#include "tree.h"
#include "taylor_func.h"
#include "derivative.h"
#include "variables.h"
#include "ctor_dtor.h"
#include "dump.h"
#include "simplifier.h"
#include "calculate_function.h"
#include "verror.h"
#include <math.h>

static int factorial(int n)
{
    if(n == 1)
    {
        return 1;
    }
    int val = factorial(n - 1);
    return val * n;
}

struct tree_node *Taylor_term(struct tree_node *node, struct variables *vars, int cur_degree, int degree)
{
    if(cur_degree > degree)
    {
        Del_tree(node);
        return Node({.type = DIGIT, .val = 0}, NULL, NULL);
    }

    struct tree_node *node_d = simplify(d(node, vars->var_arr->name));
    Del_tree(node);
    double res = 0;
    if(do_function(node_d, vars, &res))
    {
        VERROR("can't calculate function");
        return NULL;
    }
    res /= factorial(cur_degree);
    struct node_val val = {.type = OP, .op = SUB};
    struct tree_node *node_sub = Node(val, Node({.type = VAR, .var = vars->var_arr->name}, NULL, NULL), Node({.type = DIGIT, .val = vars->var_arr->val}, NULL, NULL));
    struct tree_node *node_pow = Node({.type = OP, .op = POW}, node_sub, Node({.type = DIGIT, .val = (float)cur_degree}, NULL, NULL));
    struct tree_node *taylor_term_node = Node({.type = OP, .op = MUL}, Node({.type = DIGIT, .val = res}, NULL, NULL), node_pow);
    
    return Node({.type = OP, .op = ADD}, taylor_term_node, Taylor_term(node_d, vars, cur_degree + 1, degree));
}

struct tree_node *Taylor_expansion(struct tree_node *node, struct variable *var, int degree)
{
    double res = 0;
    variables vars = {.count = 1, .var_arr = var};
    if(do_function(node, &vars, &res))
    {
        VERROR("can't calculate function");
        return 0;
    }
    struct tree_node *taylor_node = Node({.type = OP, .op = ADD}, Node({.type = DIGIT, .val = res}, NULL, NULL), Taylor_term(c(node), &vars, 1, degree));

    return taylor_node;
}
