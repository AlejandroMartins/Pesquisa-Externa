#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdio.h>
#include <stdbool.h>
#include "common_types.h"

typedef TipoRegistro Item_arq;

// #define ORDEM 2

// typedef struct pagina{
// 	int n;
// 	Item_arq registro[2 * ORDEM];
// 	struct pagina* apontador[(2 * ORDEM) + 1];
// } Pagina;

typedef struct pagina{
	int n;
	Item_arq *registro; //[2 * ordem];
	struct pagina** apontador; //[(2 * ordem) + 1];
} Pagina;

Pagina *criaPagina(int);
bool pesquisa_arvoreB(Item_arq*, Pagina*);
void inicializa_arvoreB(Pagina**);
void insere_na_pagina(Pagina*, Item_arq, Pagina*);
void ins_arvoreB(Item_arq, Pagina*, bool*, Item_arq*, Pagina**, int);
void insere_arvoreB(Item_arq, Pagina**, int);
// void reconstitui_arvoreB(Pagina*, Pagina*, int, bool*);
// // CORRIGIDO: Assinatura de antecessor_arvoreB para Item_arq* no primeiro parâmetro
// void antecessor_arvoreB(Item_arq*, int, Pagina*, bool*); 
// void ret_arvoreB(TipoChave, Pagina**, bool*);
// void retira_arvoreB(TipoChave, Pagina**);
void libera_arvoreB(Pagina*);

#endif