#include <stdio.h>
#include "tree.h"
#include "variables.h"
#include "ctor_dtor.h"
#include "parse.h"
#include "reader.h"
#include <string.h>

static int n_op = sizeof(op_names)/(sizeof(char) * max_len);

static op_t is_op(char *op_val, int word_len)
{
    for(int i = 1; i < n_op; i++)
    {
        if(!strncmp(op_names[i], op_val, (size_t)word_len))
        {
            return (op_t)i;
        }
    }
    return NONE;
}

// char *s = NULL;
// int p = 0;

struct tree_node *getE(struct parse_line *line)
{
    struct tree_node *val = getT(line);
    while(line->str[line->p] == '+' || line->str[line->p] == '-')
    {
        char op = line->str[line->p];
        (line->p)++;
        struct tree_node *val2 = getT(line);
        switch (op)
        {
            case '+':
            {
                val = Node({.type = OP, .op = ADD}, val, val2);
                break;
            }
            case '-':
            {
                val = Node({.type = OP, .op = SUB}, val, val2);
                break;
            }
            default:
                printf("error\n");
                break;
        }
    }
    return val;
}


struct tree_node *getPow(struct parse_line *line)
{
    struct tree_node *val = getP(line);
    while(line->str[line->p] == '^')
    {
        (line->p)++;
        struct tree_node *val2 = getP(line);
        val = Node({.type = OP, .op = POW}, val, val2);
    }
    return val;
}

struct tree_node *getT(struct parse_line *line)
{
    struct tree_node *val = getPow(line);
    while(line->str[line->p] == '*' || line->str[line->p] == '/')
    {
        char op = line->str[line->p];
        (line->p)++;
        struct tree_node *val2 = getPow(line);
        switch (op)
        {
            case '*':
            {
                val = Node({.type = OP, .op = MUL}, val, val2);
                break;
            }
            case '/':
            {
                val = Node({.type = OP, .op = DIV}, val, val2);
                break;
            }
            default:
                printf("error\n");
                break;
        }
    }
    return val;
}

struct tree_node *getP(struct parse_line *line)
{
    if(line->str[line->p] == '(')
    {
        (line->p)++;
        struct tree_node *val = getE(line);
        if(line->str[line->p] != ')')
        {
            printf("error\n");
            return NULL;
        }
        (line->p)++;
        return val;
    }
    else if('a' <= line->str[line->p] && line->str[line->p] <= 'z')
    {
        struct tree_node *val = getID(line);
        if(line->str[line->p] == '(')
        {
            (line->p)++;
            struct tree_node *val2 = getE(line);
            if(line->str[line->p] != ')')
            {
                printf("error\n");
                return NULL;
            }
            (line->p)++;
            val->right = val2;
            return val;
        }
        return val;
    }
    

    return getN(line);
}


struct tree_node *getID(struct parse_line *line)
{
    struct tree_node *val = Node({.type = NOTHING, .val = 0 }, NULL, NULL);
    int old_p = line->p;
    int word_i = 0;
    char tmp_var[max_len] = {};

    while(('a' <= line->str[line->p] && line->str[line->p] <= 'z') || ('A' <= line->str[line->p] && line->str[line->p] <= 'Z'))
    {
        tmp_var[word_i] = line->str[line->p];
        (line->p)++;
        word_i++;
    }

    if(old_p == line->p)
    {
        printf("error\n");
        return NULL;
    }
    op_t operation = is_op(tmp_var, word_i);

    if(operation == NONE)
    {
        val->val.var = strndup(tmp_var, word_i);
        val->val.type = VAR;
    }
    else
    {
        val->val.op = operation;
        val->val.type = OP;
    }
    return val;
}

static int is_unar_minus(struct parse_line *line)
{
    if(line->p == 0)
    {
        (line->p)++;
        return 1;
    }
    char prev = line->str[line->p - 1];
    char next = line->str[line->p + 1];

    if((prev== '+' || prev == '-' || prev == '*' || prev == '/' || prev == '(') && '0' <= next && next <= '9')
    {
        (line->p)++;
        return 1;
    }

    return 0;
}

struct tree_node *getN(struct parse_line *line)
{
    struct tree_node *val = Node({.type = DIGIT, .val = 0}, NULL, NULL);
    int old_p = line->p;
    int dot = 0;
    double mult = 1;

    int minus = 1;
    if(line->str[line->p] == '-')
    {
        if(is_unar_minus(line))
        {
            minus = -1;
        }
    }

    while(('0' <= line->str[line->p] && line->str[line->p] <= '9') || line->str[line->p] == '.')
    {

        if(line->str[line->p] == '.')
        {
            if(dot)
            {
                printf("error");
                return NULL;
            }
            dot = 1;
            (line->p)++;
            continue;
        }
        if(dot)
        {
            mult *= 0.1;
            val->val.val = val->val.val + mult * (line->str[line->p] - '0');
        }
        else
        {
            val->val.val = val->val.val * 10 + line->str[line->p] - '0';
        }
        (line->p)++;
    }

    val->val.val *= minus;

    if(old_p == line->p)
    {
        printf("error\n");
        return NULL;
    }
    return val;
}

struct tree_node *getG(char *str)
{
    struct parse_line line = {.str = str, .p = 0};

    struct tree_node *val = getE(&line);

    if(line.str[line.p] != '\0')
    {
        printf("error\n");
        return NULL;
    }
   
    return val;
}

