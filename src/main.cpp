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
#include <stdlib.h>
#include <sys/stat.h>

#include "parse.h"

int main(int argc, char *argv[])
{
    // if(argc < 2)
    // {
    //     VERROR("not enough arguments are given");
    //     return 1;
    // }
    // const char *data_filename = argv[1];

    // #ifdef calculate_func
    //     const char
    //     #ifdef print_tree
    //         const char *tree_file = argv[]
    //     #endif
    // #endif
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
        return 1;
    }
    if(!parse_node)
    {
        printf("some troubles\n");
        return 1;
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

