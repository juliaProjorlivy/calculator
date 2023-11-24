#include "dump.h"
#include "tree.h"
#include "verror.h"

void print_in_node(struct tree_node *node, char **line)
{
    int word_len = 0;
    if(!node)
    {
        return;
    }
    sprintf(*line, "(%n", &word_len);
    (*line) += word_len;
    print_in_node(node->left, line);
    if(node->type == OP)
    {
      sprintf(*line, "%s%n", node->symb, &word_len);
    }
    else (node->type == DIGIT)
    {
      sprintf(*line, "%lf%n", node->digit, &word_len);
    }
    (*line) += word_len;
    print_in_node(node->right, line);
    sprintf(*line, ")%n", &word_len);
    (*line) += word_len;
}

void node_dump(struct tree_node *node, FILE *file) // TODO: pass file as argument
{
    if(!node)
    {
        return;
    }
    fprintf(file, "    node[color=\"blue\", frontsize=14, shape=\"rectangle\", style=\"rounded, filled\", fillcolor=\"lightblue\"];\n");
    fprintf(file, "    edge[color=\"deepskyblue\",fontsize=12];\n");
    fprintf(file, "\tnode_%p[shape=record, label=\"{%s\\n |  {<f0> left | <f1> right}}\", color=\"blue\", style=\"filled\", fillcolor=\"lightgreen\"];\n", node, node->value);
    if(node->left)
    {
        fprintf(file, "\tnode_%p:<f0>->node_%p[label=\"yes\"];\n", node, node->left);
        node_dump(node->left, file);
    }
    else
    {
        fprintf(file, "\tL_NULL_%p[label=\"NULL\", color=\"red\"];\n", node);
        fprintf(file, "\tnode_%p:<f0>->L_NULL_%p;\n", node, node);
    }
    if(node->right)
    {
        fprintf(file, "\tnode_%p:<f1>->node_%p[label=\"no\"];\n", node, node->right);
        node_dump(node->right, file);
    }
    else
    {
        fprintf(file, "\tR_NULL_%p[label=\"NULL\", color=\"red\"];\n", node);
        fprintf(file, "\tnode_%p:<f1>->R_NULL_%p;\n", node, node);
    }
    
    return;
}

int tree_dump(struct tree_node *node, const char *filename)
{
    FILE *file = fopen(filename, "w");

    fprintf(file, "digraph tree{\n");
    fprintf(file, "\trankdir=TB;\n");
    node_dump(node, file);
    fprintf(file, "}\n");
    if(fclose(file))
    {
        VERROR_FCLOSE(filename);
        return 1;
    }
    return 0;
}




