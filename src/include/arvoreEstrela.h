#ifndef ARVOREESTRELA_H
#define ARVOREESTRELA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common_types.h" // Inclui TipoRegistro e TipoChave
#include "utils.h"        // Inclui contadores

// Definição de MM (número máximo de chaves em uma página interna)
// e MM2 (número máximo de registros em uma página folha).
// Baseado no conceito de ordem m, onde 2m é o máximo.
// Se a ordem m da Árvore B for 2, então MM = 2m = 4.
// Para Árvore B*, MM2 (para folhas) pode ser maior que MM (para nós internos).
#define MM 12    // (M*2)/3, M=20 Ex: Capacidade máxima de chaves em uma página interna (2m)
#define MM2 40  // M*2, M=20 Ex: Capacidade máxima de registros em uma página folha (2m' onde m' é ordem da folha)

// Tipo para identificar se a página é interna ou externa 
typedef enum {Interna, Externa} TipoIntExt;

// Declaração forward para TipoPagina, pois TipoApontador a usa 
typedef struct TipoPagina TipoPagina;

// Apontador para uma página (será um ponteiro para TipoPagina) 
typedef TipoPagina* TipoApontador;

// Estrutura da página 
struct TipoPagina {
    TipoIntExt Pt; // Indica se é interna ou externa 
    union {
        // Estrutura para páginas internas (nós de índice)
        struct {
            int ni;                     // Número de itens (chaves) na página interna 
            TipoChave ri[MM];           // Chaves 
            TipoApontador pi[MM + 1];   // Apontadores para subárvores 
        } U0;
        // Estrutura para páginas externas (folhas)
        struct {
            int ne;                     // Número de registros na página externa 
            TipoRegistro re[MM2];       // Registros completos 
        } U1;
    } UU; // União para armazenar a estrutura de página interna ou externa 
};

/**
 * @brief Inicializa a árvore B* definindo sua raiz como NULL.
 * @param Arvore Ponteiro para o apontador da raiz da árvore.
 */
void Inicializa(TipoApontador *Arvore);

/**
 * @brief Realiza a pesquisa por um registro em uma árvore B*.
 * @param x Ponteiro para um TipoRegistro contendo a chave a ser pesquisada.
 * Se encontrado, o registro completo é copiado para *x.
 * @param Ap Ponteiro para o apontador da página atual na recursão (começa pela raiz).
 */
void Pesquisa(TipoRegistro *x, TipoApontador *Ap);

/**
 * @brief Insere um novo registro na árvore B*.
 * @param Reg O registro a ser inserido.
 * @param Ap Ponteiro para o apontador da raiz da árvore.
 */
void Insere(TipoRegistro Reg, TipoApontador *Ap);

/**
 * @brief Imprime as chaves da árvore em ordem (para depuração).
 * @param arvore Apontador para a página atual (começa pela raiz).
 */
void Imprime(TipoApontador arvore);

/**
 * @brief Libera toda a memória alocada dinamicamente pela árvore B*.
 * @param arvore Apontador para a raiz da árvore.
 */
void liberaArvoreBStar(TipoApontador arvore);

void ImprimeArvore(TipoApontador Ap, int nivel);

#endif // ARVOREESTRELA_H