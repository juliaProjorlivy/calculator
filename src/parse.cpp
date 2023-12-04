#include <stdio.h>
#include "parse.h"

char *s = NULL;
int p = 0;

double getE()
{
    double val = getT();
    while(s[p] == '+' || s[p] == '-')
    {
        char op = s[p];
        p++;
        double val2 = getT();
        switch (op)
        {
            case '+':
            {
                val += val2;
                break;
            }
            case '-':
            {
                val -= val2;
                break;
            }
            default:
                printf("error\n");
                break;
        }
    }
    return val;
}

double getT()
{
    double val = getP();
    while(s[p] == '*' || s[p] == '/')
    {
        char op = s[p];
        p++;
        double val2 = getP();
        switch (op)
        {
            case '*':
            {
                val *= val2;
                break;
            }
            case '/':
            {
                val /= val2;
                break;
            }
            default:
                printf("error\n");
                break;
        }
    }
    return val;
}

double getP()
{
    if(s[p] == '(')
    {
        p++;
        double val = getE();
        if(s[p] != ')')
        {
            printf("error\n");
            return 1;
        }
        p++;
        return val;
    }
    return getN();
}

double  getN()
{
    double val = 0;
    int old_p = p;
    int dot = 0;
    int mult = 1;

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
            val = val + mult * (s[p] - '0');
        }
        else
        {
            val = 10*val + s[p] - '0';
        }
        p++;
    }
    if(old_p == p)
    {
        printf("error\n");
        return 1;
    }
    return val;
}

double getG(char *str)
{
    s = str;
    double val = getE();
    if(s[p] != '\0')
    {
        printf("error\n");
        return 1;
    }
    if((char)val == '\0')
    {
        printf("error\n");
        return 1;
    }
    return val;
}

