#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define FILENAME "arvore_binaria.dat"

// Estrutura do nó da árvore
typedef struct no_arv No_arv;


// Função para criar um novo nó
No_arv* criar_no(int);

// Função para escrever um nó no arquivo
void escrever_no(FILE*, No_arv*, long);

// Função para ler um nó do arquivo
No_arv* ler_no(FILE* arquivo, long pos);

// Função para inserir uma chave na árvore
long inserir(FILE*, int, long);

// Função para buscar uma chave na árvore
bool buscar(FILE*, int, long);