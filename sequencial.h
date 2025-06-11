#ifndef SEQUENCIAL_H
#define SEQUENCIAL_H

#include <stdio.h>
#include <stdbool.h>

typedef struct indicie_p Indicie_p;
typedef struct item_arq Item_arq;

Indicie_p* criar_indicie_pagina(FILE*, int, int);
Item_arq* criar_item_arq(int);
void liberar(Item_arq*, Indicie_p*);
bool pesquisa(Indicie_p*, int, int, Item_arq*, FILE*);





#endif // SEQUENCIAL_H