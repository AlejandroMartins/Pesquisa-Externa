#ifndef SEQUENCIAL_H
#define SEQUENCIAL_H

#include <stdio.h>
#include <stdbool.h>
#include "common_types.h"

typedef struct indicie_p{
	TipoChave chave;
} Indicie_p;

typedef TipoRegistro Item_arq;

// MODIFICADO: Adicionado 'int situacao_ordem' ao protótipo
bool pesquisa_sequencial(Indicie_p*, int, int, Item_arq*, FILE*, int situacao_ordem);

Item_arq* criar_item_arq(int tam);
Indicie_p* criar_indicie_pagina(FILE* arq, int tam_arq, int tam_pag);

#endif // SEQUENCIAL_H