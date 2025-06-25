// src/include/utils.h
#ifndef UTILS_H
#define UTILS_H

#include <time.h>   // Para clock_t, clock()
#include <stdbool.h> // Para 'bool' e 'true'/'false'

// Variáveis globais para contadores
extern long g_io_transferencias;
extern long g_comparacoes_chaves;

// Funções para gerenciar contadores
void resetar_contadores();
void incrementar_io();
void incrementar_comparacao();

// Funções para medir tempo
clock_t iniciar_tempo();
double finalizar_tempo(clock_t inicio);

// Função para calcular a ordem das árvores;
int calcula_ordem(long);

// Função para calcular mm e mm2 da b *;
int calcula_mm(int);
int calcula_mm2(int);

#endif // UTILS_H