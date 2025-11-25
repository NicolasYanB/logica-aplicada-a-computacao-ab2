#include "variable_hash.h"
#include <stdio.h>
#include <stdlib.h>

variable_hash* init_hash() {
  variable_hash *hash = (variable_hash*)malloc(sizeof(variable_hash));
  hash->keys_size = 0;
  for (int i = 0; i < MAX_KEYS; i++) hash->array[i] = NULL;
  return hash;
}

int hash_function(char key) {
  int idx;
  if (key-'A' > 25) {
    idx = key-'a'+26;
  } else {
    idx = key-'A';
  }
  return idx;
}

void put(variable_hash *hash, char key, variable *var) {
  int idx = hash_function(key);
  if (hash->array[idx] != NULL) return;
  hash->array[idx] = var;
  hash->keys[hash->keys_size++] = key;
}

variable* get(variable_hash *hash, char key) {
  int idx = hash_function(key);
  return hash->array[idx];
}