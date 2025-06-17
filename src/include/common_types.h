// src/include/common_types.h
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

// Definição comum para TipoChave
typedef long TipoChave;

// Definição comum para TipoRegistro
// Todos os métodos devem usar esta mesma estrutura
typedef struct TipoRegistro {
    TipoChave Chave;
    long dado1;
    char dado2[1000];
    char dado3[5000];
} TipoRegistro;

#endif // COMMON_TYPES_H