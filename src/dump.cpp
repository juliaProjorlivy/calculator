#include <stdio.h>
#include "tree.h"
#include "reader.h"
#include "dump.h"
#include "derivative.h"
#include "verror.h"

//print tree in line 
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
    if(node->val.type == OP)
    {
        sprintf(*line, "%s%n", op_names[node->val.op], &word_len);
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
    print_in_node(node->right, line);
    sprintf(*line, ")%n", &word_len);
    (*line) += word_len;
}

void node_dump(struct tree_node *node, FILE *file)
{
    if(!node)
    {
        return;
    }
    fprintf(file, "    node[color=\"blue\", frontsize=14, shape=\"rectangle\", style=\"rounded, filled\", fillcolor=\"lightblue\"];\n");
    fprintf(file, "    edge[color=\"deepskyblue\",fontsize=12];\n");
    switch(node->val.type)
    {
        case OP:
        {
            fprintf(file, "\tnode_%p[shape=record, label=\"{%s\\n |  {<f0> left | <f1> right}}\", color=\"blue\", style=\"filled\", fillcolor=\"lightblue\"];\n", node, op_names[node->val.op]);
            break;
        }
        case DIGIT:
        {
            fprintf(file, "\tnode_%p[shape=record, label=\"{%lf\\n |  {<f0> left | <f1> right}}\", color=\"blue\", style=\"filled\", fillcolor=\"lightgreen\"];\n", node, node->val.val);
            break;
        }
        case VAR:
        {
            fprintf(file, "\tnode_%p[shape=record, label=\"{%s\\n |  {<f0> left | <f1> right}}\", color=\"blue\", style=\"filled\", fillcolor=\"lightpink\"];\n", node, node->val.var);
            break;
        }
        case NOTHING:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    if(node->left)
    {
        fprintf(file, "\tnode_%p:<f0>->node_%p;\n", node, node->left);
        node_dump(node->left, file);
    }
    else
    {
        fprintf(file, "\tL_NULL_%p[label=\"NULL\", color=\"red\", style=\"filled\", fillcolor=\"tomato\"];\n", node);
        fprintf(file, "\tnode_%p:<f0>->L_NULL_%p;\n", node, node);
    }
    if(node->right)
    {
        fprintf(file, "\tnode_%p:<f1>->node_%p;\n", node, node->right);
        node_dump(node->right, file);
    }
    else
    {
        fprintf(file, "\tR_NULL_%p[label=\"NULL\", color=\"red\", style=\"filled\", fillcolor=\"tomato\"];\n", node);
        fprintf(file, "\tnode_%p:<f1>->R_NULL_%p;\n", node, node);
    }

    return;
}

int tree_dump(struct tree_node *node, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if(!file)
    {
        VERROR_FOPEN(filename);
        return 1;
    }

    if(fprintf(file, "digraph tree{\n\trankdir=TB;\n") < 0)
    {
        VERROR_FWRITE(filename);
        return 1;
    }

    node_dump(node, file);

    if(fprintf(file, "}\n") < 0)
    {
        VERROR_FWRITE(filename);
        return 1;
    }
    if(fclose(file))
    {
        VERROR_FCLOSE(filename);
        return 1;
    }
    return 0;
}


int latex_dump_node(struct tree_node *node, FILE *file)
{
    if(!node)
    {
        return 0;
    }
    if(node->val.type == OP)
    {
        if(node->val.op == DIV)
        {
            fprintf(file, "\\frac{");
            latex_dump_node(node->left, file);
            fprintf(file, "}{");
            latex_dump_node(node->right, file);
            fprintf(file, "}");
        }
        else if(is_un(node->val.op))
        {
            fprintf(file, "\\%s\\left(", op_names[node->val.op]);
            latex_dump_node(node->right, file);
            fprintf(file, "\\right)");
        }
        else
        {
            if(node->left)
            {
                if(node->left->left || node->left->right)
                {
                    fprintf(file, "\\left(");
                    latex_dump_node(node->left, file);
                    fprintf(file, "\\right )");
                }
            }
            else
            {
                latex_dump_node(node->left, file);
            }
            fprintf(file, "%s", op_names[node->val.op]);
            if(node->right)
            {
                if(node->right->right || node->right->left)
                {
                    fprintf(file, "\\left(");
                    latex_dump_node(node->right, file);
                    fprintf(file, "\\right )");
                }
            }
            else
            {
                latex_dump_node(node->right, file);
            }
        }
    }
    else if(node->val.type == DIGIT)
    {
        fprintf(file, " %.2lf ", node->val.val);
    }
    else
    {
        fprintf(file, " %s ", node->val.var);
    }
    
    return 0;
}

int latex_dump_tree(struct tree_node *node, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if(!file)
    {
        VERROR_FOPEN(filename);
        return 1;
    }
    fprintf(file, "\\begin{document}\n$");
    latex_dump_node(node, file);
    fprintf(file, "$\n\\end{document}\n");
    if(fclose(file))
    {
        VERROR_FCLOSE(filename);
        return 1;
    }

    return 0;
}

// need to check whether function is too big and separate her


