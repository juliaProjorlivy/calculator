#ifndef PARSE_H
#define PARSE_H

struct parse_line
{
    char *str;
    int p;
};

struct tree_node *getG(char *);

struct tree_node *getN(struct parse_line *line);

struct tree_node *getP(struct parse_line *line);

struct tree_node *getT(struct parse_line *line);

struct tree_node *getE(struct parse_line *line);

struct tree_node *getID(struct parse_line *line);

struct tree_node *getPow(struct parse_line *line);

#endif
