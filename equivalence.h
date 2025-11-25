#ifndef EQUIVALENCE_H

#define EQUIVALENCE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define prop_INIT_SIZE 1000

#define OR 124
#define AND 38
#define CONSEQUENCE 62
#define EQUIVALENCE 61
#define NOT 126

#include "node_types.h"
#include "variable_hash.h"

typedef struct proposition {
  char *string;
  size_t size;
  size_t max_size;
  variable_hash *variables;
} proposition;

proposition* initialize_proposition();
void add_char(proposition *prop, char c);
proposition* read_proposition();
variable* create_variable(char value);
connective* create_connective(char value, node *left, node *right);
negation* create_negation(node *expression);
node* create_node(void *item, enum node_type type);
bool is_connective(char token);
node* parse(proposition *prop);
node* parse_expression(proposition *prop, int *idx);
node* parse_term(proposition *prop, int *idx);
bool evaluate(node *head);
bool are_equivalents(proposition *prop1, node *tree1, proposition *prop2, node *tree2);

node* copy_subtree(node *tree);
node* negate(node* expression);
node* resolves_de_morgan_neg(negation *neg);
node* transform_consequence(connective *consequence);
node* transform_equivalence(connective *equivalence);
node* distributive_disjunction(connective *disjunction, bool left);
node* transform_formulas(node* root);
node* resolve_negations(node* root);
node* distribute_disjunctions(node* root);
node* convert_to_cnf(node* root);
node* invert_cnf(node *cnf);
node* convert_to_dnf(node *root);
void print_normal_form(node *head, bool conjunctive);

void set_interpretation(int interp[], variable_hash *hash);
int partial_evaluate(node *tree, variable_hash *hash);
bool SAT(node *tree, variable_hash *hash, int interp[], int idx);
#endif