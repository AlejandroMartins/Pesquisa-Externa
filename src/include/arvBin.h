#ifndef ARVBIN_H
#define ARVBIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common_types.h" // Inclui TipoRegistro e TipoChave

typedef struct no_arv{
    TipoChave chave;
    long int esquerda;  
    long int direita;
    long int dado1;
    char dado2[1000], dado3[5000];   
} No_arv;

// MODIFICADO: Aceita TipoRegistro para inicializar todos os dados do nó
No_arv* criar_no(TipoChave chave, TipoRegistro dados);

void escrever_no(FILE*, No_arv*, long);
No_arv* ler_no(FILE* arquivo, long pos);

// MODIFICADO: Aceita TipoRegistro para inserir o item completo
long inserir(FILE*, TipoRegistro reg, long pos_atual);

// MODIFICADO: Retorna TipoRegistro* para o main.c
bool buscar(FILE*, TipoChave chave, long pos_atual, TipoRegistro* out_reg_encontrado);

#endif // ARVBIN_H