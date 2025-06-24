#include <stdio.h>
#include <stdlib.h> // Para malloc, free, exit, EXIT_FAILURE
#include <stdbool.h> // Para bool, true, false
#include <string.h> // Para strcpy

// Includes para o contexto do seu trabalho
#include "include/arvoreEstrela.h" // Seu cabeçalho do método
#include "include/common_types.h"   // Para TipoChave e TipoRegistro
#include "include/utils.h"          // Para os contadores

// As structs TipoPagina, TipoIntExt, TipoApontador serão definidas no arvoreEstrela.h
// As constantes MM, MM2 também serão definidas no arvoreEstrela.h

#define Interna 0
#define Externa 1
#define TRUE 1
#define FALSE 0
// -----------------------------------------------------------------------------
// Implementação das funções auxiliares (static) - Baseadas no PDF e adaptadas
// -----------------------------------------------------------------------------
typedef struct {
    short Cresceu;
    int NumChavesPromovidas;
    TipoChave ChavesPromovidas[2];
    TipoApontador FilhosResultantes[3];
} ResultadoIns; 

TipoApontador CriaPagina(int tipo) {
    TipoApontador novaPagina = (TipoApontador) malloc(sizeof(TipoPagina));
    novaPagina->Pt = tipo;

    if (tipo == Interna) {
        novaPagina->UU.U0.ni = 0;
        for (int i = 0; i < MM + 1; i++) {
            novaPagina->UU.U0.pi[i] = NULL;
        }
    } else {
        novaPagina->UU.U1.ne = 0;
    }

    return novaPagina;
}


/**
 * @brief Função recursiva de inserção para Árvore B*.
 * @param Reg O registro a ser inserido.
 * @param Ap Apontador para a página atual (na recursão).
 * @param Cresceu Flag booleana que indica se a árvore cresceu em altura.
 * @param RegRetorno Registro cuja chave será promovida ao nível superior (se houver split).
 * @param ApRetorno Apontador para a nova página criada (se houver split).
 */
// SUBSTITUA A SUA FUNÇÃO 'Ins' ATUAL POR ESTA VERSÃO DE DEPURAÇÃO

// Declaração antecipada para a função recursiva (se ainda não tiver)
static ResultadoIns Ins(TipoRegistro Reg, TipoApontador Ap, TipoApontador Pai, int PosFilho);

// ... (suas outras funções como Inicializa, etc.)

ResultadoIns Ins(TipoRegistro Reg, TipoApontador Ap, TipoApontador Pai, int PosFilho) {
    ResultadoIns resultado;
    resultado.Cresceu = FALSE;
    resultado.NumChavesPromovidas = 0;

    if (Ap == NULL) {
        resultado.Cresceu = TRUE;
        resultado.NumChavesPromovidas = 1;
        resultado.ChavesPromovidas[0] = Reg.Chave;
        resultado.FilhosResultantes[0] = NULL;
        resultado.FilhosResultantes[1] = NULL;
        return resultado;
    }

    if (Ap->Pt == Interna) {
    int i = 0;
    while (i < Ap->UU.U0.ni && Reg.Chave > Ap->UU.U0.ri[i])
        i++;

    ResultadoIns subres = Ins(Reg, Ap->UU.U0.pi[i], Ap, i);

    if (!subres.Cresceu)
        return resultado;

    if (Ap->UU.U0.ni < MM) {
        for (int j = Ap->UU.U0.ni; j > i; j--) {
            Ap->UU.U0.ri[j] = Ap->UU.U0.ri[j - 1];
            Ap->UU.U0.pi[j + 1] = Ap->UU.U0.pi[j];
        }
        Ap->UU.U0.ri[i] = subres.ChavesPromovidas[0];
        Ap->UU.U0.pi[i] = subres.FilhosResultantes[0];
        Ap->UU.U0.pi[i + 1] = subres.FilhosResultantes[1];
        Ap->UU.U0.ni++;
        resultado.Cresceu = FALSE;
        return resultado;
    }

    // ----- Redistribuição e divisão de nó interno -----
    TipoApontador IrmaoEsq = NULL, IrmaoDir = NULL;
    if (Pai != NULL) {
        if (PosFilho > 0) IrmaoEsq = Pai->UU.U0.pi[PosFilho - 1];
        if (PosFilho < Pai->UU.U0.ni) IrmaoDir = Pai->UU.U0.pi[PosFilho + 1];
    }

    // --- Redistribuição com irmão esquerdo (rotação)
    if (IrmaoEsq && IrmaoEsq->UU.U0.ni < MM) {
        // Move separador do pai para Ap
        for (int j = Ap->UU.U0.ni; j > 0; j--) {
            Ap->UU.U0.ri[j] = Ap->UU.U0.ri[j - 1];
            Ap->UU.U0.pi[j + 1] = Ap->UU.U0.pi[j];
        }
        Ap->UU.U0.pi[1] = Ap->UU.U0.pi[0];
        Ap->UU.U0.ri[0] = Pai->UU.U0.ri[PosFilho - 1];
        Ap->UU.U0.pi[0] = subres.FilhosResultantes[0];
        Ap->UU.U0.ni++;

        // Irmão esquerdo doa última chave ao pai
        Pai->UU.U0.ri[PosFilho - 1] = IrmaoEsq->UU.U0.ri[IrmaoEsq->UU.U0.ni - 1];
        Ap->UU.U0.pi[0] = IrmaoEsq->UU.U0.pi[IrmaoEsq->UU.U0.ni];
        IrmaoEsq->UU.U0.ni--;

        resultado.Cresceu = FALSE;
        return resultado;
    }

    // --- Redistribuição com irmão direito (rotação)
    if (IrmaoDir && IrmaoDir->UU.U0.ni < MM) {
        // Move separador do pai para Ap
        Ap->UU.U0.ri[Ap->UU.U0.ni] = Pai->UU.U0.ri[PosFilho];
        Ap->UU.U0.pi[Ap->UU.U0.ni + 1] = subres.FilhosResultantes[0];
        Ap->UU.U0.ni++;

        // Irmão direito doa primeira chave ao pai
        Pai->UU.U0.ri[PosFilho] = IrmaoDir->UU.U0.ri[0];
        Ap->UU.U0.pi[Ap->UU.U0.ni] = IrmaoDir->UU.U0.pi[0];

        // Reorganiza irmão direito
        for (int j = 0; j < IrmaoDir->UU.U0.ni - 1; j++) {
            IrmaoDir->UU.U0.ri[j] = IrmaoDir->UU.U0.ri[j + 1];
            IrmaoDir->UU.U0.pi[j] = IrmaoDir->UU.U0.pi[j + 1];
        }
        IrmaoDir->UU.U0.pi[IrmaoDir->UU.U0.ni - 1] = IrmaoDir->UU.U0.pi[IrmaoDir->UU.U0.ni];
        IrmaoDir->UU.U0.ni--;

        resultado.Cresceu = FALSE;
        return resultado;
    }

    // --- Divisão 2-para-3 dos nós internos (com merge ordenado)
    TipoChave todosChaves[2 * MM + 1];
    TipoApontador todosPtrs[2 * MM + 2];
    int total = 0;

    // Copia chaves e ponteiros de Ap
    for (int j = 0; j < Ap->UU.U0.ni; j++)
        todosChaves[total] = Ap->UU.U0.ri[j], todosPtrs[total++] = Ap->UU.U0.pi[j];
    todosPtrs[total] = Ap->UU.U0.pi[Ap->UU.U0.ni];

    // Insere chave promovida
    todosChaves[total] = subres.ChavesPromovidas[0];
    todosPtrs[++total] = subres.FilhosResultantes[0];
    todosPtrs[++total] = subres.FilhosResultantes[1];

    // Copia chaves e ponteiros do irmão
    TipoApontador IrmaoDiv = IrmaoDir ? IrmaoDir : IrmaoEsq;
    for (int j = 0; j < IrmaoDiv->UU.U0.ni; j++)
        todosChaves[total] = IrmaoDiv->UU.U0.ri[j], todosPtrs[++total] = IrmaoDiv->UU.U0.pi[j];
    todosPtrs[++total] = IrmaoDiv->UU.U0.pi[IrmaoDiv->UU.U0.ni];

    // Divide os dados
    int partSize = (total - 1) / 3;
    TipoApontador Novo = CriaPagina(Interna);

    // Preenche Ap
    Ap->UU.U0.ni = 0;
    for (int j = 0; j < partSize; j++) {
        Ap->UU.U0.ri[j] = todosChaves[j];
        Ap->UU.U0.pi[j] = todosPtrs[j];
        Ap->UU.U0.ni++;
    }
    Ap->UU.U0.pi[Ap->UU.U0.ni] = todosPtrs[partSize];
    resultado.ChavesPromovidas[0] = todosChaves[partSize];

    // Preenche IrmaoDiv
    IrmaoDiv->UU.U0.ni = 0;
    int offset = partSize + 1;
    for (int j = 0; j < partSize; j++) {
        IrmaoDiv->UU.U0.ri[j] = todosChaves[offset + j];
        IrmaoDiv->UU.U0.pi[j] = todosPtrs[offset + j];
        IrmaoDiv->UU.U0.ni++;
    }
    IrmaoDiv->UU.U0.pi[IrmaoDiv->UU.U0.ni] = todosPtrs[offset + partSize];
    resultado.ChavesPromovidas[1]= todosChaves[offset + partSize];

    // Preenche Novo
    Novo->UU.U0.ni = 0;
    offset = offset + partSize + 1;
    int finalCount = (total - 1) - (2 * partSize + 1);
    for (int j = 0; j < finalCount; j++) {
        Novo->UU.U0.ri[j] = todosChaves[offset + j];
        Novo->UU.U0.pi[j] = todosPtrs[offset + j];
        Novo->UU.U0.ni++;
    }
    Novo->UU.U0.pi[Novo->UU.U0.ni] = todosPtrs[total];

    resultado.NumChavesPromovidas = 2;
    resultado.FilhosResultantes[0] = Ap;
    resultado.FilhosResultantes[1] = IrmaoDiv;
    resultado.FilhosResultantes[2] = Novo;
    resultado.Cresceu = TRUE;
    return resultado;
}

    else {
        int i = 0;
        while (i < Ap->UU.U1.ne && Reg.Chave > Ap->UU.U1.re[i].Chave)
            i++;

        if (i < Ap->UU.U1.ne && Reg.Chave == Ap->UU.U1.re[i].Chave) {
            printf("Registro duplicado!\\n");
            resultado.Cresceu = FALSE;
            return resultado;
        }

        if (Ap->UU.U1.ne < MM2) {
            for (int j = Ap->UU.U1.ne; j > i; j--)
                Ap->UU.U1.re[j] = Ap->UU.U1.re[j - 1];

            Ap->UU.U1.re[i] = Reg;
            Ap->UU.U1.ne++;
            resultado.Cresceu = FALSE;
            return resultado;
        }

        TipoApontador IrmaoEsq = NULL, IrmaoDir = NULL;

        if (Pai != NULL) {
            if (PosFilho > 0)
                IrmaoEsq = Pai->UU.U0.pi[PosFilho - 1];
            if (PosFilho < Pai->UU.U0.ni)
                IrmaoDir = Pai->UU.U0.pi[PosFilho + 1];

        }

        // Tentar redistribuição com o irmão esquerdo
        if (IrmaoEsq && IrmaoEsq->UU.U1.ne < MM2) {
            TipoRegistro temp[MM2 + 1];
            int nTemp = 0;

            for (int j = 0; j < Ap->UU.U1.ne; j++)
                temp[nTemp++] = Ap->UU.U1.re[j];

            temp[nTemp++] = Reg;

            // Ordena
            for (int x = 0; x < nTemp - 1; x++)
                for (int y = x + 1; y < nTemp; y++)
                    if (temp[x].Chave > temp[y].Chave) {
                        TipoRegistro aux = temp[x];
                        temp[x] = temp[y];
                        temp[y] = aux;
                    }

            // Move o menor para o final do irmão esquerdo
            IrmaoEsq->UU.U1.re[IrmaoEsq->UU.U1.ne] = temp[0];
            IrmaoEsq->UU.U1.ne++;

            // Resto fica em Ap
            for (int j = 0; j < nTemp - 1; j++)
                Ap->UU.U1.re[j] = temp[j + 1];

            Ap->UU.U1.ne = nTemp - 1;

            // Atualiza o pai
            Pai->UU.U0.ri[PosFilho - 1] = Ap->UU.U1.re[0].Chave;
            resultado.Cresceu = FALSE;
            return resultado;
        }

        // Tentar redistribuição com o irmão direito
        if (IrmaoDir && IrmaoDir->UU.U1.ne < MM2) {
            TipoRegistro temp[MM2 + 1];
            int nTemp = 0;

            for (int j = 0; j < Ap->UU.U1.ne; j++)
                temp[nTemp++] = Ap->UU.U1.re[j];

            temp[nTemp++] = Reg;

            // Ordena
            for (int x = 0; x < nTemp - 1; x++)
                for (int y = x + 1; y < nTemp; y++)
                    if (temp[x].Chave > temp[y].Chave) {
                        TipoRegistro aux = temp[x];
                        temp[x] = temp[y];
                        temp[y] = aux;
                    }

            // Mantém o primeiro em Ap
            for (int j = 0; j < nTemp - 1; j++)
                Ap->UU.U1.re[j] = temp[j];
            Ap->UU.U1.ne = nTemp - 1;


            // Move o maior para o início do irmão direito
            for (int j = IrmaoDir->UU.U1.ne; j > 0; j--)
                IrmaoDir->UU.U1.re[j] = IrmaoDir->UU.U1.re[j - 1];

            IrmaoDir->UU.U1.re[0] = temp[nTemp - 1];
            IrmaoDir->UU.U1.ne++;

            // Atualiza o pai
            Pai->UU.U0.ri[PosFilho] = IrmaoDir->UU.U1.re[0].Chave;
            resultado.Cresceu = FALSE;
            return resultado;
        }

        // Se não for possível redistribuir → Divisão 2-para-3 nas folhas
        TipoApontador IrmaoDiv = NULL;

        if (IrmaoDir)
            IrmaoDiv = IrmaoDir;

        else if (IrmaoEsq)
            IrmaoDiv = IrmaoEsq;

        else {
            printf("Erro: Nenhum irmão encontrado para divisão 2-para-3!\\n");
            resultado.Cresceu = FALSE;
            return resultado;
        }

        TipoRegistro temp[MM2 * 2 + 1];
        int nTemp = 0;

        for (int j = 0; j < Ap->UU.U1.ne; j++)
            temp[nTemp++] = Ap->UU.U1.re[j];

        for (int j = 0; j < IrmaoDiv->UU.U1.ne; j++)
            temp[nTemp++] = IrmaoDiv->UU.U1.re[j];
        temp[nTemp++] = Reg;


        // Ordena
        for (int x = 0; x < nTemp - 1; x++)
            for (int y = x + 1; y < nTemp; y++)
                if (temp[x].Chave > temp[y].Chave) {
                    TipoRegistro aux = temp[x];
                    temp[x] = temp[y];
                    temp[y] = aux;
                }

        TipoApontador Novo = CriaPagina(Externa);

        int partSize = nTemp / 3;
        int resto = nTemp % 3;
        int idx = 0;
        Ap->UU.U1.ne = 0;

        for (int j = 0; j < partSize + (resto > 0 ? 1 : 0); j++)
            Ap->UU.U1.re[Ap->UU.U1.ne++] = temp[idx++];

        IrmaoDiv->UU.U1.ne = 0;

        for (int j = 0; j < partSize + (resto > 1 ? 1 : 0); j++)
            IrmaoDiv->UU.U1.re[IrmaoDiv->UU.U1.ne++] = temp[idx++];

        Novo->UU.U1.ne = 0;

        for (int j = 0; idx < nTemp; j++)
            Novo->UU.U1.re[Novo->UU.U1.ne++] = temp[idx++];

        resultado.Cresceu = TRUE;
        resultado.NumChavesPromovidas = 2;
        resultado.ChavesPromovidas[0] = IrmaoDiv->UU.U1.re[0].Chave;
        resultado.ChavesPromovidas[1] = Novo->UU.U1.re[0].Chave;
        resultado.FilhosResultantes[0] = Ap;
        resultado.FilhosResultantes[1] = IrmaoDiv;
        resultado.FilhosResultantes[2] = Novo;
        return resultado;
    }
} 

// -----------------------------------------------------------------------------
// Implementação das funções públicas da Árvore B*
// -----------------------------------------------------------------------------

/**
 * @brief Inicializa a árvore B* definindo sua raiz como NULL.
 * @param Arvore Ponteiro para o apontador da raiz da árvore.
 */
void Inicializa(TipoApontador *Arvore) {
    *Arvore = NULL;
}

/**
 * @brief Realiza a pesquisa por um registro em uma árvore B*.
 * @param x Ponteiro para um TipoRegistro contendo a chave a ser pesquisada.
 * Se encontrado, o registro completo é copiado para *x.
 * @param Ap Ponteiro para o apontador da página atual na recursão (começa pela raiz).
 */
void Pesquisa(TipoRegistro *x, TipoApontador *Ap) {
    int i; // Índice para busca na página
    TipoApontador Pag;

    Pag = *Ap; // Dereferência o apontador da recursão para a página atual

    // CASO BASE DA RECURSÃO: Página nula (chave não encontrada)
    if (Pag == NULL) {
        x->Chave = -1; // Sinaliza que não foi encontrado. main.c irá imprimir a mensagem.
        return;
    }

    incrementar_io(); // Contagem de I/O: acesso à página (leitura)

    // Se a página atual é INTERNA (nó de índice)
    if (Pag->Pt == Interna) {
        i = 1; // Índice para busca na página interna (1-based no PDF)
        // Pesquisa sequencial na página interna para encontrar o caminho correto
        while (i < Pag->UU.U0.ni && x->Chave > Pag->UU.U0.ri[i - 1]) { // Usa 1-based para 'i' e 'i-1' para array
            incrementar_comparacao(); // Conta a comparação
            i++;
        }
        // Uma comparação final para decidir o caminho (se o loop não percorrer tudo)
        if (Pag->UU.U0.ni > 0) incrementar_comparacao();

        // Ativação recursiva: a pesquisa continua até encontrar uma página folha
        if (x->Chave < Pag->UU.U0.ri[i - 1]) { // Usa 'i-1'
            Pesquisa(x, &Pag->UU.U0.pi[i - 1]); // Usa 'i-1'
        } else {
            Pesquisa(x, &Pag->UU.U0.pi[i]); // Usa 'i'
        }
        return; // Retorna cedo se for nó interno
    }

    // Se a página atual é EXTERNA (folha)
    int idx_folha = 0; // MODIFICADO: Usar 0-based indexing para buscar na folha
    // Pesquisa sequencial na página folha para encontrar o registro
    while (idx_folha < Pag->UU.U1.ne && x->Chave > Pag->UU.U1.re[idx_folha].Chave) { // Usa 0-based 'idx_folha'
        incrementar_comparacao(); // Conta a comparação
        idx_folha++;
    }
    // Uma última comparação que decide o caminho (se o loop não percorrer tudo)
    if (Pag->UU.U1.ne > 0) incrementar_comparacao();


    // Verifica se a chave desejada foi localizada
    if (idx_folha < Pag->UU.U1.ne && x->Chave == Pag->UU.U1.re[idx_folha].Chave) { // Verifica limite e igualdade
        *x = Pag->UU.U1.re[idx_folha]; // Copia o registro completo
    } else {
        x->Chave = -1; // Sinaliza que não foi encontrado para o main.c
    }
}


void Insere(TipoRegistro Reg, TipoApontador *Ap) {
    // CASO 1: A ÁRVORE ESTÁ VAZIA — CRIA PRIMEIRA FOLHA
    if (*Ap == NULL) {
        TipoApontador novaFolha = CriaPagina(Externa);
        novaFolha->UU.U1.re[0] = Reg;
        novaFolha->UU.U1.ne = 1;
        *Ap = novaFolha;
        return;
    }

    // CASO 2: ÁRVORE EXISTE — INICIA A INSERÇÃO RECURSIVA
    ResultadoIns resultado = Ins(Reg, *Ap, NULL, 0);

    if (resultado.Cresceu) {
        TipoApontador novaRaiz = (TipoApontador)malloc(sizeof(TipoPagina));
        novaRaiz->Pt = Interna;

        if (resultado.NumChavesPromovidas == 1) {
            novaRaiz->UU.U0.ni = 1;
            novaRaiz->UU.U0.ri[0] = resultado.ChavesPromovidas[0];
            novaRaiz->UU.U0.pi[0] = *Ap;
            novaRaiz->UU.U0.pi[1] = resultado.FilhosResultantes[1];
        } else {
            novaRaiz->UU.U0.ni = 2;
            novaRaiz->UU.U0.ri[0] = resultado.ChavesPromovidas[0];
            novaRaiz->UU.U0.ri[1] = resultado.ChavesPromovidas[1];
            novaRaiz->UU.U0.pi[0] = resultado.FilhosResultantes[0];
            novaRaiz->UU.U0.pi[1] = resultado.FilhosResultantes[1];
            novaRaiz->UU.U0.pi[2] = resultado.FilhosResultantes[2];
        }

        *Ap = novaRaiz;
    }
}


/**
 * @brief Remove um registro da árvore B*.
 * @param Ch A chave do registro a ser removido.
 * @param Ap Ponteiro para o apontador da raiz da árvore.
 */
// Funções de Remoção (static para auxiliares, pois não estão no PDF de base)
static void Ret(TipoChave Ch, TipoApontador *Ap, bool *Diminuiu);
static void Reconstitui(TipoApontador ApPag, TipoApontador ApPai, int PosPai, bool *Diminuiu);

void Retira(TipoChave Ch, TipoApontador *Ap) {
    bool Diminuiu;
    TipoApontador Aux;

    Ret(Ch, Ap, &Diminuiu);

    // Se a árvore diminuiu em altura pela raiz (raiz ficou vazia)
    if (Diminuiu && (*Ap)->UU.U0.ni == 0 && (*Ap)->Pt == Interna) {
        Aux = *Ap;
        *Ap = Aux->UU.U0.pi[0]; // A nova raiz é o primeiro filho da antiga raiz
        free(Aux); // Libera a antiga raiz
    }
}

/**
 * @brief Imprime as chaves da árvore em ordem (para depuração).
 * @param arvore Apontador para a página atual (começa pela raiz).
 */
void Imprime(TipoApontador arvore){
    int i = 0;

    if (arvore == NULL) return;

    if (arvore->Pt == Interna) { // Se for página interna
        // Percorre os apontadores recursivamente
        while (i < arvore->UU.U0.ni) {
            Imprime (arvore->UU.U0.pi[i]);
            printf("%ld ", arvore->UU.U0.ri[i]); // Imprime a chave
            i++;
        }
        Imprime(arvore->UU.U0.pi[i]); // Último apontador após o loop (para a subárvore à direita da última chave)
    } else { // Se for página externa (folha)
        printf("[");
        for (i = 0; i < arvore->UU.U1.ne; i++) {
            printf("%ld ", arvore->UU.U1.re[i].Chave);
        }
        printf("] ");
    }
}

/**
 * @brief Libera toda a memória alocada dinamicamente pela árvore B*.
 * @param arvore Apontador para a raiz da árvore.
 */
void liberaArvoreBStar(TipoApontador arvore) {
    if (arvore == NULL) return;

    if (arvore->Pt == Interna) {
        for (int i = 0; i <= arvore->UU.U0.ni; i++) { // Percorre todos os apontadores
            liberaArvoreBStar(arvore->UU.U0.pi[i]);
        }
    }
    free(arvore); // Libera a própria página
}

// -----------------------------------------------------------------------------
// Implementação das funções de Remoção (Baseadas na última versão que te dei, corrigidas para serem estáticas e completas)
// -----------------------------------------------------------------------------

static void Reconstitui(TipoApontador ApPag, TipoApontador ApPai, int PosPai, bool *Diminuiu) {
    TipoApontador Aux;
    long DispAux, j;

    incrementar_io(); // Acesso à página atual
    incrementar_io(); // Acesso à página pai

    if (PosPai < ApPai->UU.U0.ni) { // Aux = TipoPagina a direita de ApPag
        Aux = ApPai->UU.U0.pi[PosPai + 1];
        incrementar_io();

        if (ApPag->Pt == Interna) {
            DispAux = (Aux->UU.U0.ni - MM / 2);
            if (DispAux > 0) {
                ApPag->UU.U0.ri[ApPag->UU.U0.ni] = ApPai->UU.U0.ri[PosPai];
                ApPag->UU.U0.pi[ApPag->UU.U0.ni + 1] = Aux->UU.U0.pi[0];
                ApPag->UU.U0.ni++;

                ApPai->UU.U0.ri[PosPai] = Aux->UU.U0.ri[0];
                
                for (j = 0; j < Aux->UU.U0.ni - 1; j++) {
                    Aux->UU.U0.ri[j] = Aux->UU.U0.ri[j + 1];
                }
                for (j = 0; j <= Aux->UU.U0.ni - 1; j++) { // Usar Aux->UU.U0.ni no loop
                    Aux->UU.U0.pi[j] = Aux->UU.U0.pi[j + 1]; // Corrigido p/ +1
                }
                Aux->UU.U0.ni--;
                *Diminuiu = false;
            } else {
                ApPag->UU.U0.ri[ApPag->UU.U0.ni] = ApPai->UU.U0.ri[PosPai];
                ApPag->UU.U0.pi[ApPag->UU.U0.ni + 1] = Aux->UU.U0.pi[0];
                ApPag->UU.U0.ni++;

                for (j = 0; j < Aux->UU.U0.ni; j++) {
                    ApPag->UU.U0.ri[ApPag->UU.U0.ni++] = Aux->UU.U0.ri[j];
                    ApPag->UU.U0.pi[ApPag->UU.U0.ni] = Aux->UU.U0.pi[j + 1];
                }
                free(Aux);

                for (j = PosPai + 1; j < ApPai->UU.U0.ni; j++) {
                    ApPai->UU.U0.ri[j - 1] = ApPai->UU.U0.ri[j];
                    ApPai->UU.U0.pi[j] = ApPai->UU.U0.pi[j + 1];
                }
                ApPai->UU.U0.ni--;
                *Diminuiu = (ApPai->UU.U0.ni < MM / 2);
            }
        } else { // Reconstituição de página externa (folha)
            DispAux = (Aux->UU.U1.ne - MM2 / 2);
            if (DispAux > 0) {
                for (j = 0; j < DispAux; j++) {
                    ApPag->UU.U1.re[ApPag->UU.U1.ne++] = Aux->UU.U1.re[j];
                }
                ApPai->UU.U0.ri[PosPai] = Aux->UU.U1.re[DispAux].Chave;
                
                for (j = 0; j < Aux->UU.U1.ne - DispAux; j++) {
                    Aux->UU.U1.re[j] = Aux->UU.U1.re[j + DispAux];
                }
                Aux->UU.U1.ne -= DispAux;
                *Diminuiu = false;
            } else {
                for (j = 0; j < Aux->UU.U1.ne; j++) {
                    ApPag->UU.U1.re[ApPag->UU.U1.ne++] = Aux->UU.U1.re[j];
                }
                free(Aux);

                for (j = PosPai + 1; j < ApPai->UU.U0.ni; j++) {
                    ApPai->UU.U0.ri[j - 1] = ApPai->UU.U0.ri[j];
                    ApPai->UU.U0.pi[j] = ApPai->UU.U0.pi[j + 1];
                }
                ApPai->UU.U0.ni--;
                *Diminuiu = (ApPai->UU.U0.ni < MM / 2);
            }
        }

    } else { // Aux = TipoPagina a esquerda de ApPag
        Aux = ApPai->UU.U0.pi[PosPai - 1];
        incrementar_io();

        if (ApPag->Pt == Interna) {
            DispAux = (Aux->UU.U0.ni - MM / 2);
            if (DispAux > 0) {
                for (j = ApPag->UU.U0.ni; j >= 0; j--) {
                    ApPag->UU.U0.ri[j] = ApPag->UU.U0.ri[j - 1];
                }
                for (j = ApPag->UU.U0.ni + 1; j >= 0; j--) {
                    ApPag->UU.U0.pi[j] = ApPag->UU.U0.pi[j - 1];
                }
                ApPag->UU.U0.ni++;

                ApPag->UU.U0.ri[0] = ApPai->UU.U0.ri[PosPai - 1];
                ApPag->UU.U0.pi[0] = Aux->UU.U0.pi[Aux->UU.U0.ni];

                ApPai->UU.U0.ri[PosPai - 1] = Aux->UU.U0.ri[Aux->UU.U0.ni - 1];
                Aux->UU.U0.ni--;
                *Diminuiu = false;
            } else {
                Aux->UU.U0.ri[Aux->UU.U0.ni] = ApPai->UU.U0.ri[PosPai - 1];
                Aux->UU.U0.pi[Aux->UU.U0.ni + 1] = ApPag->UU.U0.pi[0];
                Aux->UU.U0.ni++;

                for (j = 0; j < ApPag->UU.U0.ni; j++) {
                    Aux->UU.U0.ri[Aux->UU.U0.ni++] = ApPag->UU.U0.ri[j];
                    Aux->UU.U0.pi[Aux->UU.U0.ni] = Aux->UU.U0.pi[j + 1];
                }
                free(ApPag);
                for (j = PosPai; j < ApPai->UU.U0.ni; j++) {
                    ApPai->UU.U0.ri[j - 1] = ApPai->UU.U0.ri[j];
                    ApPai->UU.U0.pi[j] = ApPai->UU.U0.pi[j + 1];
                }
                ApPai->UU.U0.ni--;
                *Diminuiu = (ApPai->UU.U0.ni < MM / 2);
            }
        } else {
            DispAux = (Aux->UU.U1.ne - MM2 / 2);
            if (DispAux > 0) {
                for (j = ApPag->UU.U1.ne; j >= 0; j--) {
                    ApPag->UU.U1.re[j] = ApPag->UU.U1.re[j - 1];
                }
                ApPag->UU.U1.ne++;

                ApPag->UU.U1.re[0] = Aux->UU.U1.re[Aux->UU.U1.ne - DispAux];
                ApPai->UU.U0.ri[PosPai - 1] = ApPag->UU.U1.re[0].Chave;
                Aux->UU.U1.ne -= DispAux;
                *Diminuiu = false;
            } else {
                for (j = 0; j < Aux->UU.U1.ne; j++) {
                    Aux->UU.U1.re[Aux->UU.U1.ne++] = Aux->UU.U1.re[j];
                }
                free(ApPag);
                for (j = PosPai; j < ApPai->UU.U0.ni; j++) {
                    ApPai->UU.U0.ri[j - 1] = ApPai->UU.U0.ri[j];
                    ApPai->UU.U0.pi[j] = ApPai->UU.U0.pi[j + 1];
                }
                ApPai->UU.U0.ni--;
                *Diminuiu = (ApPai->UU.U0.ni < MM / 2);
            }
        }
    }
}

static void Ret(TipoChave Ch, TipoApontador *Ap, bool *Diminuiu) {
    long j, Ind = 1; // Ind é o índice de busca (1-based)
    TipoApontador Pag;

    Pag = *Ap; // Página atual na recursão

    if (Pag == NULL) {
        printf("Erro: registro com chave %ld nao esta na arvore\n", Ch);
        *Diminuiu = false; // Não diminuiu (não havia nada para remover)
        return;
    }
    incrementar_io(); // Conta o acesso à página

    // Se a página é INTERNA (nó de índice)
    if (Pag->Pt == Interna) {
        // Encontra o índice da subárvore para descer
        while (Ind < Pag->UU.U0.ni && Ch > Pag->UU.U0.ri[Ind - 1]) { // Usa 1-based 'Ind' e 'Ind-1' para array
            incrementar_comparacao();
            Ind++;
        }
        if (Pag->UU.U0.ni > 0) incrementar_comparacao(); // Última comparação do loop ou condição
        
        bool chave_exata_no_indice = (Ind <= Pag->UU.U0.ni && Ch == Pag->UU.U0.ri[Ind - 1]);

        // Chamada recursiva para descer na árvore
        if (chave_exata_no_indice) { // Se a chave está no nó interno (guia), desce pela direita
            Ret(Ch, &Pag->UU.U0.pi[Ind], Diminuiu); // Continua a busca na subárvore à direita
        } else { // Caso contrário, desce pela subárvore apropriada (esquerda ou direita)
            if (Ch < Pag->UU.U0.ri[Ind - 1]) {
                Ret(Ch, &Pag->UU.U0.pi[Ind - 1], Diminuiu); // Desce pela esquerda
            } else {
                Ret(Ch, &Pag->UU.U0.pi[Ind], Diminuiu); // Desce pela direita
            }
        }

        // Se a subárvore retornou que diminuiu, precisa reconstituir
        if (*Diminuiu) {
            // Reconstitui a página filho que diminuiu
            // O índice `Ind` ou `Ind-1` deve ser passado corretamente.
            if (chave_exata_no_indice) { // Se a chave estava no índice e afetou o filho direito
                 Reconstitui(Pag->UU.U0.pi[Ind], *Ap, Ind, Diminuiu);
            } else { // Se a chave não estava no índice
                if (Ch < Pag->UU.U0.ri[Ind - 1]) {
                     Reconstitui(Pag->UU.U0.pi[Ind - 1], *Ap, Ind - 1, Diminuiu);
                } else {
                     Reconstitui(Pag->UU.U0.pi[Ind], *Ap, Ind, Diminuiu);
                }
            }
        }
        
    } else { // Se a página é EXTERNA (folha)
        // Encontra o registro a ser removido na folha
        while (Ind < Pag->UU.U1.ne && Ch > Pag->UU.U1.re[Ind - 1].Chave) { // Usa 1-based 'Ind' e 'Ind-1' para array
            incrementar_comparacao();
            Ind++;
        }
        if (Pag->UU.U1.ne > 0) incrementar_comparacao(); // Última comparação do loop ou condição

        // Se a chave foi encontrada na folha
        if (Ind <= Pag->UU.U1.ne && Ch == Pag->UU.U1.re[Ind - 1].Chave) { // Verifica limite e igualdade
            Pag->UU.U1.ne--; // Decrementa o número de registros
            // Desloca os registros para preencher o espaço do removido
            for (j = Ind - 1; j < Pag->UU.U1.ne; j++) {
                Pag->UU.U1.re[j] = Pag->UU.U1.re[j + 1];
            }
            // Verifica se a propriedade 'm' foi violada (página ficou com menos do mínimo de itens)
            *Diminuiu = (Pag->UU.U1.ne < (MM2 / 2));
            return;
        } else {
            printf("Erro: Registro com chave %ld nao esta presente na arvore para remocao.\n", Ch);
            *Diminuiu = false; // Não diminuiu (não removeu nada)
            return;
        }
    }
}