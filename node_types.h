#ifndef NODE_H

#define NODE_H

#include <stdbool.h>

enum node_type {
  VARIABLE,
  CONNECTIVE,
  NEGATION
};

typedef struct Node {
  void *item;
  enum node_type type;
} node;

typedef struct Connective {
  char value;
  node *left;
  node *right;
} connective;

typedef struct Negation {
  node *argument;
} negation;

typedef struct Variable {
  char value;
  int eval;
} variable;

#endif