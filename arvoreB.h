#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdio.h>
#include <stdbool.h>

typedef struct item_arq Item_arq;

typedef struct pagina Pagina;

bool pesquisa(Item_arq*, Pagina*);
void inicializa(Pagina*);
void insere_na_pagina(Pagina*, Item_arq, Pagina*);
void ins(Item_arq, Pagina*, int*, Item_arq*, Pagina*);
void insere(Item_arq, Pagina*);
void reconstitui(Pagina*, Pagina*, int, int*);
void antecessor(Pagina*, int, Pagina*, int*);
void ret(long int, Pagina*, int*);
void retira(long int, Pagina*);



#endif