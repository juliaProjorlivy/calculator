#include <stdio.h>
#include "tree.h"
#include "variables.h"
#include "ctor_dtor.h"
#include "parse.h"

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
                val->val.val += val2->val.val;
                Del_tree(val2);
                // val += val2;
                break;
            }
            case '-':
            {
                val->val.val -= val2->val.val;
                Del_tree(val2);
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

struct tree_node *getT()
{
    struct tree_node *val = getP();
    while(s[p] == '*' || s[p] == '/')
    {
        char op = s[p];
        p++;
        struct tree_node *val2 = getP();
        switch (op)
        {
            case '*':
            {
                // struct tree_node *res = Node({.type = OP, .op = MUL}, val, val2);
                val->val.val = val->val.val * val2->val.val;
                Del_tree(val2);
                // val *= val2;
                break;
            }
            case '/':
            {
                val->val.val = val->val.val / val2->val.val;
                Del_tree(val2);
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
    return getN();
}

struct tree_node *getID()
{
    struct tree_node *val = Node({.type = VAR, .var = 0}, NULL, NULL);
    int old_p = p;
    int word_i = 0;

    while('a' <= s[p] && s[p] <= 'z')
    {
        val->val.var[word_i] = s[p];
        p++;
        word_i++;
    }
    if(old_p == p)
    {
        printf("error\n");
        return NULL;
    }
    return val;
}

struct tree_node *getN()
{
    struct tree_node *val = Node({.type = DIGIT, .val = 0}, NULL, NULL);
    int old_p = p;
    int dot = 0;
    double mult = 1;

    while(('0' <= s[p] && s[p] <= '9') || s[p] == '.')
    {
        if(s[p] == '.')
        {
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
    if((char)(val->val.val) == '\0')
    {
        printf("error\n");
        return NULL;
    }
    return val;
}

