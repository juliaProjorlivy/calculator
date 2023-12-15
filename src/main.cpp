#include <stdio.h>
#include <stdarg.h>
#include "tree.h"
#include "variables.h"
#include "ctor_dtor.h"
#include "dump.h"
#include "reader.h"
#include "derivative.h"
#include "calculate_function.h"
#include "simplifier.h"
#include "verror.h"
#include "taylor_func.h"
#include "parse.h"
#include "io.h"
#include <stdlib.h>
#include <sys/stat.h>

char *get_line_from_file(const char *filename);

char *get_line_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r+");
    struct stat buf;
    if(stat(filename, &buf))
    {
        VERROR("could not stat");
        return NULL;
    }
    size_t data_size = (size_t)buf.st_size + 1;
    char *line = (char *)calloc(sizeof(char), data_size);
    if(line == NULL)
    {
        VERROR_MEM;
        return NULL;
    }

    if(fread(line, (size_t)buf.st_size, 1, file) < 1)
    {
        VERROR_FWRITE(filename);
        return NULL;
    } 
    if(fclose(file))
    {
        VERROR_FCLOSE(filename);
        return NULL;
    }

    return line;
}

// #define SECOND

const int max_data_size = 500;

#ifndef SECOND
int main(int argc, char *argv[])
{
    switch(argc - 1)
    {
        case 0:
            VERROR("too little arguments are given");
            return 1;
        case 1:
        {
            char *line = (char *)calloc(sizeof(char), max_data_size);
            if(!line)
            {
                VERROR_MEM;
                return 1;
            }
            if(scanf("%s", line) <= 0)
            {
                VERROR("troubles reading the line");
                free(line);
                return 1;
            }
            struct tree_node *node = getG(line);
            if(!node)
            {
                free(line);
                return 1;
            }
            if(!strcmp(argv[1], "-plt"))
            {
                VERROR("need output file name passed as a second parametr");
                free(line);
                Del_tree(node);
                return 1;
            }
            if(process_request(argv[1], node, 0))
            {
                free(line);
                Del_tree(node);
                return 1;
            }
            free(line);
            Del_tree(node);
            return 0;
        }
        case 2:
        {
            if(!strcmp(argv[1], "-plt") )
            {
                char *line = (char *)calloc(sizeof(char), max_data_size);
                if(!line)
                {
                    VERROR_MEM;
                    return 1;
                }
                if(scanf("%s", line) <= 0)
                {
                    VERROR("can't read the data");
                    free(line);
                    return 1;
                }
                struct tree_node *node = getG(line);
                if(!node)
                {
                    free(line);
                    return 1;
                }
                if(process_request(argv[1], node, 2, argv[2], line))
                {
                    free(line);
                    Del_tree(node);
                    return 1;
                }
                free(line);
                Del_tree(node);
                return 0;
            }
            else 
            {
                char *request = (argv[1][0] == '-') ? argv[1] : argv[2];
                char *data_filename = (argv[1][0] != '-') ? argv[1] : argv[2];
                char *line = get_line_from_file(data_filename);
                if(!line)
                {
                    return 1;
                }
                struct tree_node *node = getG(line);
                if(!node)
                {
                    free(line);
                    return 1;
                }
                if(process_request(request, node, 0))
                {
                    free(line);
                    Del_tree(node);
                    return 1;
                }
                free(line);
                Del_tree(node);
                return 0;
            }
        }
        case 3:
        {
            int plot_first = (!strcmp(argv[1], "-plt")) ? 1 : 0;
            char *outfname = plot_first ? argv[2] : argv[3];
            char *request = plot_first ? argv[3] : argv[1];
            char *line = (char *)calloc(sizeof(char), max_data_size);
            if(!line)
            {
                VERROR_MEM;
                return 1;
            }
            if(scanf("%s", line) <= 0)
            {
                VERROR("troubles reading the line");
                return 1;
            }
            struct tree_node *node = getG(line);
            if(!node)
            {
                free(line);
                return 1;
            }
            if(process_request(request, node, 2, outfname, line))
            {
                free(line);
                Del_tree(node);
                return 1;
            }

            Del_tree(node);
            free(line);
            return 0;
        }
        case 4:
        {
            int plot_first = (!strcmp(argv[1], "-plt")) ? 1 : 0;
            int plot_second = (!strcmp(argv[2], "-plt")) ? 1 : 0;
            int data_fname_first = (argv[1][0] != '-') ? 1 : 0;
            int data_fname_second = (argv[2][0] != '-' && !plot_first) ? 1 : 0;
            int data_fname_third = (argv[3][0] != '-' && !plot_second) ? 1 : 0;

            char *data_fname = (data_fname_first) ? argv[1] : ((data_fname_second) ? argv[2] : ((data_fname_third) ? argv[3] : argv[4]));

            char *outfname = (plot_first) ? argv[2] : ((plot_second) ? argv[3] : argv[4]);

            char *request = (plot_first && data_fname_third) ? argv[4] : ((plot_first) ? argv[3] : ((plot_second && data_fname_first) ? argv[2] : argv[1]));

            char *line = get_line_from_file(data_fname);
            if(!line)
            {
                VERROR_MEM;
                return 1;
            }
            struct tree_node *node = getG(line);
            if(!node)
            {
                free(line);
                return 1;
            }
            if(process_request(request, node, 2, outfname, line))
            {
                free(line);
                Del_tree(node);
                return 1;
            }
            free(line);
            Del_tree(node);
            return 0;
        }
        default:
            VERROR("unexpected number of arguments");
            return 1;
    }

    return 0;
}
#endif

#ifdef SECOND
int main(int argc, char *argv[])
{


    const char *data_filename = "data.txt";
    const char *dot_filename = "graph/graph.dot";
    const char *taylor_filename = "taylor.tex";

    FILE *file = fopen(data_filename, "r+");
    struct stat buf;
    if(stat(data_filename, &buf))
    {
        VERROR("could not stat");
        return 1;
    }
    size_t data_size = (size_t)buf.st_size + 1;
    char *line = (char *)calloc(sizeof(char), data_size);
    if(line == NULL)
    {
        VERROR_MEM;
        return 1;
    }

    if(fread(line, (size_t)buf.st_size, 1, file) < 1)
    {
        VERROR_FWRITE(data_filename);
        return 1;
    } 
    if(fclose(file))
    {
        VERROR_FCLOSE(data_filename);
        return 1;
    }
    printf("line = %s\n", line);

    char *ptr_line = line;
    char *new_line = (char *)calloc(sizeof(char), 2 * data_size);
    char *ptr_new_line = new_line;

    struct tree_node *root = reader(&line);
    print_in_node(root, &new_line, &ptr_new_line);
    printf("new_line = %s\n", ptr_new_line);
    tree_dump(root, dot_filename);

    struct variables *vars = Variables(2, "time", 0.);
    double result = 0;
    do_function(root, vars, &result);
    printf("res = %lf\n", result);

    char var[] = "time";
    struct tree_node *derivative = d(root, var);
    derivative = simplify(derivative);
    do_function(derivative, vars, &result);
    char *der_line = (char *)calloc(sizeof(char), 500);
    char *ptr_der_line = der_line;
    print_in_node(derivative, &der_line, &ptr_der_line);
    printf("der_line = %s\n", ptr_der_line);
    printf("derivative = %lf\n", result);
    const char *dot_filename_2 = "graph/dgraph.dot";

    const char *latex = "graph/derivative.tex";
    latex_dump_tree(derivative, latex);

    struct tree_node *taylor_node = Taylor_expansion(root, vars->var_arr, 3);
    taylor_node = simplify(taylor_node);
    latex_dump_tree(taylor_node, taylor_filename);
    tree_dump(taylor_node, "graph/taylor.dot");

    char linee[30] = {};
    scanf("%s", linee);
    struct tree_node *parse_node = getG(linee);
    if(!parse_node)
    {
        VERROR("INCORRECT SYNTAX");
    }
    double res_parse = 0;
    if(do_function(parse_node, vars, &res_parse))
    {
        printf("do func bad\n");
    }
    if(!parse_node)
    {
        printf("some troubles\n");
    }
    printf("%lf\n", res_parse);
    Del_tree(parse_node);

    tree_dump(derivative, dot_filename_2);
    Del_tree(root);
    Del_tree(derivative);
    Del_Variables(vars);
    free(ptr_line);
    free(ptr_new_line);
    free(ptr_der_line);
    Del_tree(taylor_node);


    return 0;
}
#endif
