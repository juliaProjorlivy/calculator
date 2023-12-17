#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "tree.h"
#include "io.h"
#include "variables.h"
#include "calculate_function.h"
#include "derivative.h"
#include "simplifier.h"
#include "dump.h"
#include "ctor_dtor.h"
#include "taylor_func.h"
#include "verror.h"
#include "reader.h"

struct variables *vars_process()
{
    printf("write a number of variables and then all of them with their value(8 x=9, y = 8 z = 88)\n");
    int n_vars = 0;
    if(scanf("%d", &n_vars) <= 0)
    {
        VERROR("expected number of variables");
        return NULL;
    }
    struct variables *vars = (struct variables *)calloc(sizeof(struct variables), 1);
    if(!vars)
    {
        VERROR_MEM;
        return NULL;
    }
    vars->var_arr = (struct variable *)calloc(sizeof(struct variable), n_vars);
    if(!vars->var_arr)
    {
        VERROR_MEM;
        free(vars);
        return NULL;
    }
    
    const int max_len = 10;
    char var[max_len] = {};
    double var_val = 0;
    int word_len = 0;
    for(int i_var = 0; i_var < n_vars; i_var++)
    {
        scanf("%*[\n ]%[a-zA-Z]%n", var, &word_len);
        
        vars->var_arr[i_var].name = strdup(var);
        scanf("%*[^0-9.]%lf", &(vars->var_arr[i_var].val));
        vars->count++;
    }
    
    return vars;
}


void to_gnuplot_format(struct tree_node *node, char **line)
{
    int word_len = 0;
    if(!node)
    {
        return;
    }
    sprintf(*line, "(%n", &word_len);
    (*line) += word_len;
    to_gnuplot_format(node->left, line);
    if(node->val.type == OP)
    {
        if(node->val.op == POW)
        {
            sprintf(*line, "**%n", &word_len);
        }
        else if(node->val.op == LN)
        {
            sprintf(*line, "log%n", &word_len);
        }
        else
        {
            sprintf(*line, "%s%n", op_names[node->val.op], &word_len);
        }
    }
    else if(node->val.type == DIGIT)
    {
        sprintf(*line, "%lf%n", node->val.val, &word_len);
    }
    else 
    {
        sprintf(*line, "%s%n", node->val.var, &word_len);
    }
    (*line) += word_len;
    to_gnuplot_format(node->right, line);
    sprintf(*line, ")%n", &word_len);
    (*line) += word_len;
}


int plot(char *outfname, int n_graphs, ...)
{

    FILE *file = popen("gnuplot", "w");
    if(!file)
    {
        VERROR("troubles opening the program");
        return 1;
    }
    if(fprintf(file, "set terminal png\nset output \'%s\'\n", outfname) <= 0)
    {
        VERROR("can't write in program");
        return 1;
    }

    va_list lines;
    va_start(lines, n_graphs);
    fprintf(file, "plot ");
    for(int i_line = 0; i_line < n_graphs; i_line++)
    {
        struct tree_node *node = va_arg(lines, struct tree_node *);
        // char *str = va_arg(lines, char *);
        char *gnuplot_format_str = (char *)calloc(sizeof(char), node_size(node));
        if(!gnuplot_format_str)
        {
            VERROR_MEM;
            return 1;
        }
        char *gnuplot_format_str_ptr = gnuplot_format_str;
        to_gnuplot_format(node, &gnuplot_format_str);
        if(fprintf(file, "%s,", gnuplot_format_str_ptr) <= 0)
        {
            free(gnuplot_format_str_ptr);
            VERROR("can't write the program");
            if(pclose(file) == -1)
            {
                VERROR("can't close the program");
            }
            return 1;
        }
        free(gnuplot_format_str_ptr);
    }
    fprintf(file, "\n");
    va_end(lines);

    if(pclose(file) == -1)
    {
        VERROR("can't close the program");
        return 1;
    }
    return 0;
}

int fval(struct tree_node *node, int n_lines, va_list lines)
{
    struct variables *vars = vars_process();
    if(!vars)
    {
        return 1;
    }
    
    double res = 0;
    if(do_function(node, vars, &res))
    {
        Del_Variables(vars);
        VERROR("can't calculate function");
        return 1;
    }
    
    if(n_lines)
    {
        char *outfname = va_arg(lines, char *); // output file name
        plot(outfname, 1, node);
    }
    
    Del_Variables(vars);
    printf("answer = %lf\n", res);
    return 0;
}

int fder(struct tree_node *node, int n_lines, va_list lines)
{
    printf("enter an output file:\n");
    const int max_len = 50;
    char fname[max_len] = {};
    if(scanf("%s", fname) <=0)
    {
        VERROR("troubles reading the filename");
        return 1;
    }

    printf("enter a variable:\n");
    char var[max_len] = {};
    if(scanf("%s", var) <= 0)
    {
        VERROR("toubles reading variable");
        return 1;
    }

    struct tree_node *der = d(node, var);
    der = simplify(der);
    if(!der)
    {
        return 1;
    }
    if(latex_dump_tree(der, fname))
    {
        Del_tree(der);
        return 1;
    }
    
    if(n_lines)
    {
        char *outfname = va_arg(lines, char *);
        plot(outfname, 2, node, der);
    }

    Del_tree(der);
    return 0;
}

int ftay(struct tree_node *node, int n_lines, va_list lines)
{
    const int max_len = 50;
    int degree = 0;
    double x0 = 0;
    char var[max_len] = {};
    char fname[max_len] = {};
    printf("degree: ");
    if(scanf("%d", &degree) <= 0)
    {
        VERROR("troubles reading the degree");
        return 1;
    }
    printf("x0: ");
    if(scanf("%lf", &x0) <= 0)
    {
        VERROR("troubles reading x0");
        return 1;
    }
    printf("var: ");
    if(scanf("%s", var) <= 0)
    {
        VERROR("troubles reading the variable");
        return 1;
    }
    printf("fname: ");
    if(scanf("%s", fname) <= 0)
    {
        VERROR("troubles reading the filename");
        return 0;
    }
    printf("\n");
    struct variable whole_var = {.name = var, .val = x0};
    struct tree_node *tay = simplify(Taylor_expansion(node, &whole_var, degree));
    if(!tay)
    {
        VERROR("can't calculate taylor expansion");
        return 1;
    }

    if(latex_dump_tree(tay, fname))
    {
        VERROR("can't write taylor expansion");
        Del_tree(tay);
        return 1;
    }

    if(n_lines)
    {
        char *outfname = va_arg(lines, char *);
        plot(outfname, 2, node, tay);
    }
    
    Del_tree(tay);
    return 0;
}


int process_request(char *request, struct tree_node *node, int n_lines, ...)
{
    va_list lines;
    va_start(lines, n_lines);
    
    if(!strcmp(request, "-fval"))
    {
        if(fval(node, n_lines, lines))
        {
            va_end(lines);
            return 1;
        }
    }
    else if(!strcmp(request, "-fder"))
    {
        if(fder(node, n_lines, lines))
        {
            va_end(lines);
            return 1;
        }
    }
    else if(!strcmp(request, "-ftay"))
    {
        if(ftay(node, n_lines, lines))
        {
            va_end(lines);
            return 1;
        }
    }
    else if(!strcmp(request, "-plt"))
    {
        if(plot(va_arg(lines, char *), 1, va_arg(lines, char *)))
        {
            VERROR("troubles plotting");
            va_end(lines);
            return 1;
        }
    }
    else 
    {
        va_end(lines);
        VERROR("unexpected argument");
        return 1;
    }

    va_end(lines);
    return 0;
}


