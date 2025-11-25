# Objetivo
Fazer um programa que faça as seguintes tarefas:
- (i)                          Dadas duas sentenças da lógica proposicional, verificar, algebricamente (usando a tabela de equivalências vistas em sala de aula), se elas são equivalentes ou não;

- (ii)                       Dada uma sentença qualquer da lógica proposicional, gerar uma sentença equivalente que esteja na Forma Normal Conjuntiva (FNC);

- (iii)                    Dada uma sentença qualquer da lógica proposicional, gerar uma sentença equivalente que esteja na Forma Normal Disjuntiva (FND);

- (iv)                     Usando a sentença gerada em (ii), verificar se ela é satisfatível (algo na linha de um SAT solver).

# Sintaxe utilizada
Como nos programas feitos para a AB1, esse programa segue as seguintes convenções:

  
  - Poderão ser usados qualquer caractere do alfabeto para as variáveis (maiusculas ou minusculas)
  - A negação é representada como o caractere '~'
  - disjunção: '|'
  - conjunção: '&'
  - condicional (se então): '>'
  - equivalência (se e somente se): '='

# Como rodar

dado as dependência do make instaladas no computador rode ```make```

Em seguida será criada uma pasta build com o executável principal dentro, assim execute no terminal ```build/main``` para iniciar o programa

