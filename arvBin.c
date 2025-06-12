#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "arvBin.h"

#define FILENAME "arvore_binaria.dat"

// Estrutura do nó da árvore
struct no_arv{
    int chave;
    long int esquerda;  
    long int direita;
    long int dado1;
    char dado2[1000], dado3[5000];   
};

// Função para criar um novo nó
No_arv* criar_no(int chave) {
    No_arv* novo = malloc(sizeof(No_arv));
    novo->chave = chave;
    novo->esquerda = -1;  // -1 indica nenhum filho
    novo->direita = -1;
    return novo;
}

// Função para escrever um nó no arquivo
void escrever_no(FILE* arquivo, No_arv* no, long pos) {
    if (arquivo == NULL || no == NULL) 
        return;
    
    fseek(arquivo, pos, SEEK_SET);
    fwrite(no, sizeof(No_arv), 1, arquivo);
}

// Função para ler um nó do arquivo
No_arv* ler_no(FILE* arquivo, long pos) {
    if (arquivo == NULL) 
        return NULL;
    
    No_arv* no = malloc(sizeof(No_arv));
    fseek(arquivo, pos, SEEK_SET);
    fread(no, sizeof(No_arv), 1, arquivo);
    return no;
}

// Função para inserir uma chave na árvore
long inserir(FILE* arquivo, int chave, long pos_atual) {
    if (pos_atual == -1) 
    {
        // Criar novo nó no final do arquivo
        No_arv* novo = criar_no(chave);
        fseek(arquivo, 0, SEEK_END);
        long nova_pos = ftell(arquivo);
        escrever_no(arquivo, novo, nova_pos);
        free(novo);
        return nova_pos;
    }
    
    No_arv* atual = ler_no(arquivo, pos_atual);
    
    if (chave < atual->chave) 
    {
        atual->esquerda = inserir(arquivo, chave, atual->esquerda);
        escrever_no(arquivo, atual, pos_atual);
    } 
    else if (chave > atual->chave) 
    {
        atual->direita = inserir(arquivo, chave, atual->direita);
        escrever_no(arquivo, atual, pos_atual);
    }
    
    free(atual);
    return pos_atual;
}

// Função para buscar uma chave na árvore
bool buscar(FILE* arquivo, int chave, long pos_atual, No_arv* no) {
    if (pos_atual == -1) 
        return false;
    
    No_arv* atual = ler_no(arquivo, pos_atual);
    bool encontrado = false;
    
    if (chave == atual->chave) 
    {
        encontrado = true;
        no = atual;
    } 
    else if (chave < atual->chave) 
    {
        encontrado = buscar(arquivo, chave, atual->esquerda, no);
    } 
    else 
    {
        encontrado = buscar(arquivo, chave, atual->direita, no);
    }
    
    free(atual);
    return encontrado;
}

