#include "equivalence.h"
#include <stdbool.h>
#include <stdlib.h>

proposition* initialize_proposition() {
  proposition *prop = (proposition*)malloc(sizeof(proposition));
  prop->max_size = prop_INIT_SIZE;
  prop->string = (char*)calloc(prop->max_size, sizeof(char));
  prop->size = 0;
  prop->variables = init_hash();
  return prop;
}

void add_char(proposition *prop, char c) {
  prop->string[prop->size] = c;
  prop->size++;
  if (prop->size > prop->max_size*0.9) {
    size_t new_size = prop->size + (prop->size / 2);
    prop->string = (char*)realloc(prop->string, new_size);
    prop->max_size = new_size;
  }
  prop->string[prop->size] = '\0';
}

proposition* read_proposition() {
  char c;
  proposition *new_prop = initialize_proposition();
  do {
    c = getchar();
    if (c == ' ' || c == '\n') continue;
    add_char(new_prop, c);
  } while (c != '\n');
  return new_prop;
}

variable* create_variable(char value) {
  variable *var = (variable*)malloc(sizeof(variable));
  var->value = value;
  var->eval = false;
  if (value == '1') {
    var->eval = true;
  }
  return var;
}

connective* create_connective(char value, node *left, node *right) {
  connective *conn = (connective*)malloc(sizeof(connective));
  conn->value = value;
  conn->left = left;
  conn->right = right;
  return conn;
}

negation* create_negation(node* expression) {
  negation *neg = (negation*)malloc(sizeof(negation));
  neg->argument = expression;
  return neg;
}

node* create_node(void *item, enum node_type type) {
  node *new_node = (node*)malloc(sizeof(node));
  new_node->item = item;
  new_node->type = type;
  return new_node;
}

bool is_connective(char token) {
  return (token == OR || token == AND || token == CONSEQUENCE || token == EQUIVALENCE);
}

node* parse(proposition *prop) {
  int idx = 0;
  return parse_expression(prop, &idx);
}

node* parse_expression(proposition *prop, int *idx) {
  char token = prop->string[*idx];
  node *term_a;
  if (token == '(') {
    (*idx)++;
    term_a = parse_expression(prop, idx);
    if (prop->string[*idx] != ')') return NULL;
    (*idx)++;
  } else {
    term_a = parse_term(prop, idx);
  }
  token = prop->string[*idx];
  char connective_value;
  while (true) {
    if (is_connective(token)) {
      connective_value = token;
      (*idx)++;
      node* term_b = parse_term(prop, idx);
      connective *conn = create_connective(connective_value, term_a, term_b);
      term_a = create_node(conn, CONNECTIVE);
      token = prop->string[*idx];
    } else {
      return term_a;
    }
  }
}

node* parse_term(proposition *prop, int *idx) {
  char token = prop->string[*idx];
  if ((token >= 65 && token <= 90) || (token >= 97 && token <= 122) || token == '1' || token == '0') {
    variable *var = get(prop->variables, token);
    if (var == NULL) {
      var = create_variable(token);
      put(prop->variables, token, var);
    }
    node *new_node = create_node(var, VARIABLE);
    (*idx)++;
    return new_node;
  } else if (token == '(') {
    return parse_expression(prop, idx);
  } else if (token == NOT) {
    (*idx)++;
    node *term = parse_term(prop, idx);
    negation *neg = create_negation(term);
    return create_node(neg, NEGATION);
  } else {
    return NULL;
  }
}

bool evaluate(node *head) {
  if (head->type == VARIABLE) {
    variable *var = (variable*)(head->item);
    return var->eval;
  } else if (head->type == NEGATION) {
    negation *neg = (negation*)(head->item);
    return !evaluate(neg->argument);
  } else if (head->type == CONNECTIVE) {
    connective *conn = (connective*)(head->item);
    int left_eval = evaluate(conn->left);
    int right_eval = evaluate(conn->right);
    switch (conn->value) {
      case OR:
        return (left_eval || right_eval);
      case AND:
        return (left_eval && right_eval);
      case CONSEQUENCE:
        return (!left_eval || right_eval);
      case EQUIVALENCE:
        return ((left_eval && right_eval) || (!left_eval && ! right_eval));
      default:
        return -1;
    }
  }
}

bool are_equivalents(proposition *prop1, node *tree1, proposition *prop2, node *tree2) {
  variable_hash *vars1 = prop1->variables;
  variable_hash *vars2 = prop2->variables;
  int nvars = vars1->keys_size;
  char key_union[MAX_KEYS];
  for (int i = 0; i < vars1->keys_size; i++) {
    key_union[i] = vars1->keys[i];
  }
  for (int i = 0; i < vars2->keys_size; i++) {
    if (get(vars1, vars2->keys[i]) == NULL) {
      key_union[nvars++] = vars2->keys[i];
    }
  }

  int total = 1 << nvars;
  char key;
  bool value;
  variable *var1, *var2;
  for (int mask = 0; mask < total; mask++) {
    for (int i = 0; i < nvars; i++) {
      key = key_union[i];
      value = (mask >> i) & 1;
      var1 = get(vars1, key);
      var2 = get(vars2, key);
      if (var1 != NULL) var1->eval = value;
      if (var2 != NULL) var2->eval = value;
    }
    if (evaluate(tree1) != evaluate(tree2)) return false;
  }
  return true;
}

// Conversion to CNF
node* copy_subtree(node *tree) {
  node* new_node;
  // copy item
  switch (tree->type) {
    case VARIABLE:
      return create_node(tree->item, VARIABLE);
    case NEGATION:
      negation *neg = (negation*)(tree->item);
      negation *new_neg = create_negation(copy_subtree(neg->argument));
      new_node = create_node(
        new_neg,
        NEGATION
      );
      break;
    case CONNECTIVE:
      connective *conn = (connective*)(tree->item);
      connective *new_conn = create_connective(
        conn->value,
        copy_subtree(conn->left),
        copy_subtree(conn->right)
      );
      new_node = create_node(
        new_conn,
        CONNECTIVE
      );
      break;
    default:
      return NULL;
  }
  return new_node;
}

node* negate(node* expression) {
  if (expression->type == NEGATION) {
    negation* neg = (negation*)expression->item;
    return copy_subtree(neg->argument);
  } else {
    return create_node(
      create_negation(copy_subtree(expression)),
      NEGATION
    );
  }
}

void destroy_tree(node* root) {
  if (root->type == NEGATION) {
    negation *neg = (negation*)(root->item);
    destroy_tree(neg->argument);
    free(neg);
  }
  if (root->type == CONNECTIVE) {
    connective *conn = (connective*)(root->item);
    destroy_tree(conn->left);
    destroy_tree(conn->right);
    free(conn);
  }
  free(root);
}

node* resolves_de_morgan_neg(negation *neg) {
  connective *conn = (connective*)neg->argument->item;
  connective *new_conn = conn->value == '&' ? create_connective('|', NULL, NULL) : create_connective('&', NULL, NULL);

  new_conn->left = negate(conn->left);
  new_conn->right = negate(conn->right);

  return create_node(new_conn, CONNECTIVE);
}

node* distributive_disjunction(connective *disjunction, bool left) {
  connective *conjunction = left ? (connective*)(disjunction->left->item) : (connective*)(disjunction->right->item);
  node *distributed_term = left ? disjunction->right : disjunction->left;
  connective *and = create_connective('&', NULL, NULL);
  connective *term_1_or = create_connective('|', NULL, NULL);
  connective *term_2_or = create_connective('|', NULL, NULL);

  term_1_or->left = copy_subtree(conjunction->left);
  term_1_or->right = copy_subtree(distributed_term);

  term_2_or->left = copy_subtree(conjunction->right);
  term_2_or->right = copy_subtree(distributed_term);

  and->left = create_node(term_1_or, CONNECTIVE);
  and->right = create_node(term_2_or, CONNECTIVE);

  return create_node(and, CONNECTIVE);
}

node* transform_consequence(connective *consequence) {
  connective *or = create_connective('|', NULL, NULL);
  node *term_a = negate(consequence->left);
  node *term_b = copy_subtree(consequence->right);
  or->left = term_a;
  or->right = term_b;
  return create_node(or, CONNECTIVE);
}

node* transform_equivalence(connective *equivalence) {
  connective *or = create_connective('|', NULL, NULL);
  connective *term_1_and = create_connective('&', NULL, NULL);
  connective *term_2_and = create_connective('&', NULL, NULL);

  term_1_and->left = copy_subtree(equivalence->left);
  term_1_and->right = copy_subtree(equivalence->right);

  term_2_and->left = negate(equivalence->left);
  term_2_and->right = negate(equivalence->right);

  or->left = create_node(term_1_and, CONNECTIVE);
  or->right = create_node(term_2_and, CONNECTIVE);

  return create_node(or, CONNECTIVE);
}

node* transform_formulas(node *root) {
  if (root->type == VARIABLE) return root;
  if (root->type == NEGATION) {
    negation *neg = (negation*)(root->item);
    node *expr = neg->argument;
    neg->argument = transform_formulas(neg->argument);
  }
  if (root->type == CONNECTIVE) {
    connective *conn = (connective*)(root->item);
    node *left = conn->left, *right = conn->right;
    conn->left = transform_formulas(conn->left);
    conn->right = transform_formulas(conn->right);

    node *og_root = root;
    if (conn->value == '=') {
      root = transform_equivalence(conn);
      destroy_tree(og_root);
    } else if (conn->value == '>') {
      root = transform_consequence(conn);
      destroy_tree(og_root);
    }
  }

  return root;
}

node* resolve_negations(node *root) {
  if (root->type == VARIABLE) return root;
  if (root->type == NEGATION) {
    node *og_root = root;
    negation *neg = (negation*)(root->item);
    if (neg->argument->type == VARIABLE) return root;
    node* expr = neg->argument;
    neg->argument = resolve_negations(neg->argument);
    root = resolves_de_morgan_neg(neg);
    destroy_tree(og_root);
  }
  if (root->type == CONNECTIVE) {
    connective *conn = (connective*)(root->item);
    conn->left = resolve_negations(conn->left);
    conn->right = resolve_negations(conn->right);
  }
  return root;
}

node* distribute_disjunctions(node *root) {
  if (root->type == VARIABLE || root->type == NEGATION) return root;
  connective *conn = (connective*)(root->item);
  conn->left = distribute_disjunctions(conn->left);
  conn->right = distribute_disjunctions(conn->right);

  node *og_root = root;
  if (conn->value == '|') {
    node *left = conn->left, *right = conn->right;
    if (left->type == CONNECTIVE) {
      connective *left_conn = (connective*)(left->item);
      if (left_conn->value == '&') {
        root = distributive_disjunction(conn, true);
        destroy_tree(og_root);
        return distribute_disjunctions(root);
      }
    }
    if (right->type == CONNECTIVE) {
      connective *right_conn = (connective*)(right->item);
      if (right_conn->value == '&') {
        root = distributive_disjunction(conn, false);
        destroy_tree(og_root);
        return distribute_disjunctions(root);
      }
    }
  }

  return root;
}

node* convert_to_cnf(node *root) {
  node *normalized = transform_formulas(root);
  node *after_de_morgan = resolve_negations(normalized);
  return distribute_disjunctions(after_de_morgan);
}

node *invert_cnf(node *cnf) {
  if (cnf->type == VARIABLE) {
    return negate(cnf);
  }
  if (cnf->type == NEGATION) {
    negation *neg = (negation*)(cnf->item);
    return neg->argument;
  }
  if (cnf->type == CONNECTIVE) {
    connective *conn = (connective*)(cnf->item);
    char next_value = conn->value == '&' ? '|' : '&';
    conn->value = next_value;
    conn->left = invert_cnf(conn->left);
    conn->right = invert_cnf(conn->right);
    return cnf;
  }
}

node *convert_to_dnf(node *root) {
  node *negated_cnf = convert_to_cnf(negate(root));
  return invert_cnf(negated_cnf);
}

void print_normal_form(node *head, bool conjunctive) {
  char comp = conjunctive ? '&' : '|';
  char inverse = conjunctive ? '|' : '&';
  if (head->type == CONNECTIVE) {
    connective *conn = (connective*)(head->item);
    if (conn->value == comp) {

      if (conn->left->type != CONNECTIVE) {
        printf("(");
        print_normal_form(conn->left, conjunctive);
        printf(")");
      } else {
        connective *left_conn = (connective*)(conn->left->item);
        if (left_conn->value == inverse) {
          printf("(");
          print_normal_form(conn->left, conjunctive);
          printf(")");
        } else {
          print_normal_form(conn->left, conjunctive);
        }
      }

      printf(" %c ", conn->value);

      if (conn->right->type != CONNECTIVE) {
        printf("(");
        print_normal_form(conn->right, conjunctive);
        printf(")");
      } else {
        connective *right_conn = (connective*)(conn->right->item);
        if (right_conn->value == inverse) {
          printf("(");
          print_normal_form(conn->right, conjunctive);
          printf(")");
        } else {
          print_normal_form(conn->right, conjunctive);
        }
      }
    } else {
      print_normal_form(conn->left, conjunctive);
      printf(" %c ", conn->value);
      print_normal_form(conn->right, conjunctive);
    }
  }
  if (head->type == NEGATION) {
    negation *neg = (negation*)(head->item);
    printf("~");
    print_normal_form(neg->argument, conjunctive);
  }
  if (head->type == VARIABLE) {
    variable *var = (variable*)(head->item);
    printf("%c", var->value);
  }
}

void set_interpretation(int interp[], variable_hash *hash) {
  int size = hash->keys_size;
  for (int i = 0; i < size; i ++) {
    char key = hash->keys[i];
    hash->array[hash_function(key)]->eval = interp[i];
  }
}

int partial_evaluate(node *tree, variable_hash *hash) {
  if (tree->type == VARIABLE) {
    variable *var = (variable*)(tree->item);
    return hash->array[hash_function(var->value)]->eval;
  }
  if (tree->type == NEGATION) {
    negation *neg = (negation*)(tree->item);
    int val = partial_evaluate(neg->argument, hash);
    if (val == -1) return -1;
    else return !val;
  }
  if (tree->type == CONNECTIVE) {
    connective *conn = (connective*)(tree->item);
    int left_val, right_val;
    if (conn->value == '&') {
      left_val = partial_evaluate(conn->left, hash);
      if (left_val == 0) return 0;
      if (left_val == -1) return -1;

      right_val = partial_evaluate(conn->right, hash);
      if (right_val == 0) return 0;
      if (right_val == -1) return -1;

      return 1;
    } else if (conn->value == '|') {
      left_val = partial_evaluate(conn->left, hash);
      if (left_val == 1) return 1;

      right_val = partial_evaluate(conn->right, hash);
      if (right_val == 1) return 1;

      if (right_val == -1 || left_val == -1) return -1;
      if (right_val == 0 && left_val == 0) return 0;
    }
  }
}

bool SAT(node *tree, variable_hash *hash, int interp[], int idx) {
  set_interpretation(interp, hash);
  int eval = partial_evaluate(tree, hash);
  if (eval != -1) return eval;

  int next_eval;
  interp[idx] = 0;
  next_eval = SAT(tree, hash, interp, idx+1);
  if (next_eval == 1) return true;

  interp[idx] = 1;
  next_eval = SAT(tree, hash, interp, idx+1);
  if (next_eval == 1) return true;

  return false;
}