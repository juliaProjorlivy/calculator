#include "ctor_dtor.h"
#include "dump.h"
#include "reader.h"
#include "func.h"
#include "verror.h"
#include <stdlib.h>
#include <sys/stat.h>

int main()
{
    const char *data_filename = "data.txt";
    const char *dot_filename = "graph/graph.dot";

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
    char *ptr_new_line= new_line;

    struct tree_node *root = reader(&line);
    print_in_node(root, &new_line);
    tree_dump(root, dot_filename);

    char var2[5] = "x";
    char var1[5] = "time";
    struct variables *vars = Variables(4, var1, var2);
    double result = 0;
    do_function(root, vars, &result);
    printf("res = %lf\n", result);

    Del_tree(root);
    Del_Variables(vars);
    free(ptr_line);
    free(ptr_new_line);


    return 0;
}

