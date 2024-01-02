#ifndef IO_H
#define IO_H

typedef enum
{
    NOT_PLOT = 0,
    PLOT = 1,
} plot_t;

struct variables *vars_process();

int fval(struct tree_node *node, int n_lines, va_list);

int fder(struct tree_node *node, int n_lines, va_list);

int ftay(struct tree_node *node, int n_lines, va_list);

int process_request(char *request, struct tree_node *node, int n_lines, ...);

int plot(char *outfname, int n_graphs, ...);

void to_gnuplot_format(struct tree_node *node, char **line);


#endif

