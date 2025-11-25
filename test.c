#include <stdio.h>
#include "equivalence.h"

void equivalence() {
  proposition *prop1, *prop2;
  printf("Insira a sentença 1: ");
  prop1 = read_proposition();
  printf("Insira a sentença 2: ");
  prop2 = read_proposition();

  node* parse_tree_1 = parse(prop1);
  node *parse_tree_2 = parse(prop2);

  bool result = are_equivalents(prop1, parse_tree_1, prop2, parse_tree_2);
  if (result) {
    printf("As duas sentenças são equivalentes!\n");
  } else {
    printf("As duas sentenças não são equivalences\n");
  }
}

void to_cnf() {
  printf("Insira a sentença a ser convertida para CNF: ");
  proposition *prop = read_proposition();
  node *parse_tree = parse(prop);
  node *cnf = convert_to_cnf(parse_tree);
  printf("Formula em CNF: \n");
  print_normal_form(cnf, true);
  printf("\n");
}

void to_dnf() {
  printf("Insira a sentença a ser convertida para DNF: ");
  proposition *prop = read_proposition();
  node *parse_tree = parse(prop);
  node *cnf = convert_to_dnf(parse_tree);
  printf("Formula em DNF: \n");
  print_normal_form(cnf, false);
  printf("\n");
}

void test_sat() {
  printf("Insira a sentença para saber se ela é satisfatível ou não: ");
  proposition *prop = read_proposition();
  node *parse_tree = parse(prop);
  node *cnf = convert_to_cnf(parse_tree);
  int size = prop->variables->keys_size;
  int interp[size];
  for (int i = 0; i < size; i++) interp[i] = -1;
  bool result = SAT(cnf, prop->variables, interp, 0);
  if (result) {
    printf("A sentença é SAT!\n");
  } else {
    printf("A sentença é UNSAT!\n");
  }
}

int main() {
  printf("Escolha a opção:\n");
  printf("1 - Equivalencia entre duas formulas\n");
  printf("2 - Conversão para forma normal conjuntiva (CNF)\n");
  printf("3 - Conversão para forma normal disjuntiva (DNF)\n");
  printf("4 - Teste de satisfabilidade (SAT)\n");
  int op;
  scanf("%d", &op);
  getchar();
  switch (op) {
    case 1:
      equivalence();
      break;
    case 2:
      to_cnf();
      break;
    case 3:
      to_dnf();
      break;
    case 4:
      test_sat();
      break;
    default:
      exit(0);
  }
  return 0;
}