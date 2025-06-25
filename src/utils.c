// src/utils.c
#include "include/utils.h"
#include <stdio.h> // Para NULL em alguns sistemas

long g_io_transferencias = 0;
long g_comparacoes_chaves = 0;

void resetar_contadores() {
    g_io_transferencias = 0;
    g_comparacoes_chaves = 0;
}

void incrementar_io() {
    g_io_transferencias++;
}

void incrementar_comparacao() {
    g_comparacoes_chaves++;
}

clock_t iniciar_tempo() {
    return clock();
}

double finalizar_tempo(clock_t inicio) {
    return (double)(clock() - inicio) / CLOCKS_PER_SEC;
}

int calcula_ordem(long q) {
    return q * 0.05;
}

int calcula_mm(int m) {
    return (2 * m) / 3;
}

int calcula_mm2(int m) {
    return 2 * m;
}