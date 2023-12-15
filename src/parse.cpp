#include <stdio.h>
#include "tree.h"
#include "variables.h"
#include "ctor_dtor.h"
#include "parse.h"
#include "reader.h"
#include "verror.h"
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

struct tree_node *getE(struct parse_line *line)
{
    struct tree_node *val = getT(line);

    if(!val)
    {
        return NULL;
    }

    while(line->str[line->p] == '+' || line->str[line->p] == '-')
    {
        int old_p = line->p;
        char op = line->str[line->p];
        (line->p)++;
        struct tree_node *val2 = getT(line);

        if(!val2)
        {
            Del_tree(val);
            return NULL;
        }

        switch (op)
        {
            case '+':
            {
                struct tree_node *tmp_val = Node({.type = OP, .op = ADD}, val, val2);
                if(!tmp_val)
                {
                    Del_tree(val);
                    Del_tree(val2);
                    VERROR_MEM;
                    return NULL;
                }
                val = tmp_val;
                break;
            }
            case '-':
            {
                struct tree_node *tmp_val = Node({.type = OP, .op = SUB}, val, val2);
                if(!tmp_val)
                {
                    Del_tree(val);
                    Del_tree(val2);
                    VERROR_MEM;
                    return NULL;
                }
                val = tmp_val;
                break;
            }
            default:
                Del_tree(val);
                Del_tree(val2);
                VERROR("syntax error in %d position of the \"%s\"\nexpected \"+\" or \"-\"", old_p, line->str);
                break;
        }
    }
    return val;
}


struct tree_node *getPow(struct parse_line *line)
{
    struct tree_node *val = getP(line);

    if(!val)
    {
        return NULL;
    }

    while(line->str[line->p] == '^')
    {
        (line->p)++;
        struct tree_node *val2 = getP(line);

        if(!val2)
        {
            Del_tree(val);
            return NULL;
        }

        struct tree_node *tmp_val = Node({.type = OP, .op = POW}, val, val2);
        if(!tmp_val)
        {
            Del_tree(val);
            Del_tree(val2);
            VERROR_MEM;
            return NULL;
        }
        val = tmp_val;
    }
    return val;
}

struct tree_node *getT(struct parse_line *line)
{
    struct tree_node *val = getPow(line);

    if(!val)
    {
        return NULL;
    }

    while(line->str[line->p] == '*' || line->str[line->p] == '/')
    {
        int old_p = line->p;
        char op = line->str[line->p];
        (line->p)++;
        struct tree_node *val2 = getPow(line);

        if(!val2)
        {
            Del_tree(val);
            return NULL;
        }

        switch (op)
        {
            case '*':
            {
                struct tree_node *tmp_val = Node({.type = OP, .op = MUL}, val, val2);
                if(!tmp_val)
                {
                    VERROR_MEM;
                    Del_tree(val);
                    Del_tree(val2);
                    return NULL;
                }
                val = tmp_val;
                break;
            }
            case '/':
            {
                struct tree_node *tmp_val = Node({.type = OP, .op = DIV}, val, val2);
                if(!tmp_val)
                {
                    VERROR_MEM;
                    Del_tree(val);
                    Del_tree(val2);
                    return NULL;
                }
                val = tmp_val;
                break;
            }
            default:
            {
                VERROR("syntax error in %d position of the \"%s\"\nexpected \"*\" or \"/\"", old_p, line->str);
                Del_tree(val);
                Del_tree(val2);
                return NULL;
            }
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

        if(!val)
        {
            return NULL;
        }

        if(line->str[line->p] != ')')
        {
            VERROR("syntax error in %d position of the \"%s\"\nexpected \")\"", line->p, line->str);
            Del_tree(val);
            return NULL;
        }
        (line->p)++;
        return val;
    }
    else if('a' <= line->str[line->p] && line->str[line->p] <= 'z')
    {
        struct tree_node *val = getID(line);

        if(!val)
        {
            return NULL;
        }

        if(line->str[line->p] == '(')
        {
            (line->p)++;
            struct tree_node *val2 = getE(line);

            if(!val2)
            {
                Del_tree(val);
                return NULL;
            }

            if(line->str[line->p] != ')')
            {
                VERROR("syntax error in %d position of the \"%s\"\nexpected \")\"", line->p, line->str);
                Del_tree(val);
                Del_tree(val2);
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

    if(!val)
    {
        VERROR_MEM;
        return NULL;
    }

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
        VERROR("syntax error in %d position of the \"%s\"\nexpected ['a'-'z''A'-'Z']", line->p, line->str);
        Del_tree(val);
        return NULL;
    }
    op_t operation = is_op(tmp_var, word_i);

    if(operation == NONE)
    {
        val->val.var = strndup(tmp_var, (size_t)word_i);

        if(!val->val.var)
        {
            VERROR_MEM;
            Del_tree(val);
            return NULL;
        }

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

    if(!val)
    {
        VERROR_MEM;
        return NULL;
    }

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
                VERROR("syntax error in %d position of the \"%s\"\nexpected a digit", line->p, line->str);
                Del_tree(val);
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
        VERROR("syntax error in %d position of the \"%s\"\nexpected digit", line->p, line->str);
        Del_tree(val);
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
        VERROR("syntax error in %d position of the \"%s\"\nexpected '\\0'", line.p, line.str);
        Del_tree(val);
        return NULL;
    }
   
    return val;
}

