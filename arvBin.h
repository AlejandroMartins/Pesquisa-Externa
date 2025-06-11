#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define FILENAME "arvore_binaria.dat"

// Estrutura do nó da árvore
typedef struct no_arv No_arv;


// Função para criar um novo nó
TreeNode* criar_no(int);

// Função para escrever um nó no arquivo
void escrever_no(FILE*, TreeNode*, long);

// Função para ler um nó do arquivo
TreeNode* ler_no(FILE* arquivo, long pos);

// Função para inserir uma chave na árvore
long inserir(FILE*, int, long);

// Função para buscar uma chave na árvore
bool buscar(FILE*, int, long);