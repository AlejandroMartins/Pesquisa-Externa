#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdio.h>
#include <stdbool.h>
#include "common_types.h"

typedef TipoRegistro Item_arq;

#define ORDEM 20

typedef struct pagina{
	int n;
	Item_arq registro[2 * ORDEM];
	struct pagina* apontador[(2 * ORDEM) + 1];
} Pagina;

bool pesquisa_arvoreB(Item_arq*, Pagina*);
void inicializa_arvoreB(Pagina**);
void insere_na_pagina(Pagina*, Item_arq, Pagina*);
void ins_arvoreB(Item_arq, Pagina*, bool*, Item_arq*, Pagina**);
void insere_arvoreB(Item_arq, Pagina**);
void libera_arvoreB(Pagina*);

#endif