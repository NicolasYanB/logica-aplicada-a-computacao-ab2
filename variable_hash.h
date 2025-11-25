#ifndef VAR_HASH_H

#define VAR_HASH_H

#include "node_types.h"

#define MAX_KEYS 52

typedef struct VariableHash {
  char keys[MAX_KEYS];
  variable* array[MAX_KEYS];
  int keys_size;
} variable_hash;

variable_hash* init_hash();
int hash_function(char key);
void put(variable_hash *hash, char key, variable *var);
variable* get(variable_hash *hash, char key);

#endif