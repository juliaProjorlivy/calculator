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

char *s = NULL;
int p = 0;

struct tree_node *getE()
{
    struct tree_node *val = getT();
    while(s[p] == '+' || s[p] == '-')
    {
        char op = s[p];
        p++;
        struct tree_node *val2 = getT();
        switch (op)
        {
            case '+':
            {
                val = Node({.type = OP, .op = ADD}, val, val2);
                // val->val.val += val2->val.val;
                // Del_tree(val2);
                // val += val2;
                break;
            }
            case '-':
            {
                val = Node({.type = OP, .op = SUB}, val, val2);
                // val->val.val -= val2->val.val;
                // Del_tree(val2);
                // val -= val2;
                break;
            }
            default:
                printf("error\n");
                break;
        }
    }
    return val;
}


struct tree_node *getPow()
{
    struct tree_node *val = getP();
    while(s[p] == '^')
    {
        p++;
        struct tree_node *val2 = getP();
        val = Node({.type = OP, .op = POW}, val, val2);
    }
    return val;
}

struct tree_node *getT()
{
    struct tree_node *val = getPow();
    while(s[p] == '*' || s[p] == '/')
    {
        char op = s[p];
        p++;
        struct tree_node *val2 = getPow();
        switch (op)
        {
            case '*':
            {
                val = Node({.type = OP, .op = MUL}, val, val2);
                // val->val.val = val->val.val * val2->val.val;
                // Del_tree(val2);
                // val *= val2;
                break;
            }
            case '/':
            {
                val = Node({.type = OP, .op = DIV}, val, val2);
                // val->val.val = val->val.val / val2->val.val;
                // Del_tree(val2);
                // val /= val2;
                break;
            }
            default:
                printf("error\n");
                break;
        }
    }
    return val;
}

struct tree_node *getP()
{
    if(s[p] == '(')
    {
        p++;
        struct tree_node *val = getE();
        if(s[p] != ')')
        {
            printf("error\n");
            return NULL;
        }
        p++;
        return val;
    }
    else if('a' <= s[p] && s[p] <= 'z')
    {
        struct tree_node *val = getID();
        if(s[p] == '(')
        {
            p++;
            struct tree_node *val2 = getE();
            if(s[p] != ')')
            {
                printf("error\n");
                return NULL;
            }
            p++;
            val->right = val2;
            return val;
        }
        return val;
    }
    

    return getN();
}


struct tree_node *getID()
{
    struct tree_node *val = Node({.type = NOTHING, .val = 0 }, NULL, NULL);
    int old_p = p;
    int word_i = 0;
    char tmp_var[max_len] = {};

    while(('a' <= s[p] && s[p] <= 'z') || ('A' <= s[p] && s[p] <= 'Z'))
    {
        tmp_var[word_i] = s[p];
        p++;
        word_i++;
    }

    if(old_p == p)
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

struct tree_node *getN()
{
    struct tree_node *val = Node({.type = DIGIT, .val = 0}, NULL, NULL);
    int old_p = p;
    int dot = 0;
    double mult = 1;

    int minus = 1;
    if(s[p] == '-')
    {
        if(p == 0)
        {
            p++;
            minus = -1;
        }
        if(p > 0)
        {
            if((s[p-1] == '+' || s[p-1] == '-' || s[p-1] == '*' || s[p-1] == '/' || s[p-1] == '(') && '0' <= s[p+1] && s[p+1] <= '9')
            {
                p++;
                minus = -1;
            }
        }
    }

    while(('0' <= s[p] && s[p] <= '9') || s[p] == '.')
    {

        if(s[p] == '.')
        {
            if(dot)
            {
                printf("error");
                return NULL;
            }
            dot = 1;
            p++;
            continue;
        }
        if(dot)
        {
            mult *= 0.1;
            val->val.val = val->val.val + mult * (s[p] - '0');

            // val = val + mult * (s[p] - '0');

        }
        else
        {
            val->val.val = val->val.val * 10 + s[p] - '0';
            // val = 10*val + s[p] - '0';
        }
        p++;
    }

    val->val.val *= minus;

    if(old_p == p)
    {
        printf("error\n");
        return NULL;
    }
    return val;
}

struct tree_node *getG(char *str)
{
    s = str;
    struct tree_node *val = getE();
    if(s[p] != '\0')
    {
        printf("error\n");
        return NULL;
    }
    // if((char)(val->val.val) == '\0')
    // {
    //     printf("error\n");
    //     return NULL;
    // }
    return val;
}

