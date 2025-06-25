#include <stdio.h>
#include <stdlib.h> // Para malloc, free, exit, EXIT_FAILURE
#include <stdbool.h> // Para bool, true, false
#include <string.h> // Para strcpy

// Includes para o contexto do seu trabalho
#include "include/arvoreEstrela.h" // Seu cabeçalho do método
#include "include/common_types.h"   // Para TipoChave e TipoRegistro
#include "include/utils.h"          // Para os contadores e ordem

// As structs TipoPagina, TipoIntExt, TipoApontador serão definidas no arvoreEstrela.h

#define Interna 0
#define Externa 1
#define TRUE 1
#define FALSE 0

typedef struct {
    short Cresceu;
    int NumChavesPromovidas;
    TipoChave ChavesPromovidas[2];
    TipoApontador FilhosResultantes[3];
} ResultadoIns; 

TipoApontador CriaPagina(int tipo, int mm, int mm2) {
    TipoApontador novaPagina = (TipoApontador) malloc(sizeof(TipoPagina));
    novaPagina->Pt = tipo;

    if (tipo == Interna) {
        novaPagina->UU.U0.ni = 0;
        novaPagina->UU.U0.ri = (TipoChave*) malloc(mm*sizeof(TipoChave));
        novaPagina->UU.U0.pi = (TipoApontador*) malloc((mm+1)*sizeof(TipoApontador));
        for (int i = 0; i < mm + 1; i++) {
            novaPagina->UU.U0.pi[i] = NULL;
        }
    } else {
        novaPagina->UU.U1.re = (TipoRegistro*) malloc(mm2*sizeof(TipoRegistro));
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
static ResultadoIns Ins(TipoRegistro Reg, TipoApontador Ap, TipoApontador Pai, int PosFilho, int mm, int mm2);

// ... (suas outras funções como Inicializa, etc.)

ResultadoIns Ins(TipoRegistro Reg, TipoApontador Ap, TipoApontador Pai, int PosFilho, int mm, int mm2) {
    ResultadoIns resultado;
    resultado.Cresceu = FALSE;
    resultado.NumChavesPromovidas = 0;

    // Caso base: nó nulo (cria nova folha)
    if (Ap == NULL) {
        resultado.Cresceu = TRUE;
        resultado.NumChavesPromovidas = 1;
        resultado.ChavesPromovidas[0] = Reg.Chave;
        resultado.FilhosResultantes[0] = NULL;
        resultado.FilhosResultantes[1] = NULL;
        return resultado;
    }

    // Nó interno
    if (Ap->Pt == Interna) {
        int i = 0;
        while (i < Ap->UU.U0.ni && Reg.Chave > Ap->UU.U0.ri[i])
            i++;

        // Verifica duplicata em nó interno
        if (i < Ap->UU.U0.ni && Reg.Chave == Ap->UU.U0.ri[i]) {
            printf("Erro: Chave  já existe no nó interno!\n");
            resultado.Cresceu = FALSE;
            return resultado;
        }

        ResultadoIns subres = Ins(Reg, Ap->UU.U0.pi[i], Ap, i, mm, mm2);

        if (!subres.Cresceu)
            return resultado;

        // Tem espaço no nó interno
        if (Ap->UU.U0.ni + subres.NumChavesPromovidas <= mm) {
            // move para abrir espaço
            for (int j = Ap->UU.U0.ni - 1 + subres.NumChavesPromovidas; j >= i + subres.NumChavesPromovidas; j--) {
                Ap->UU.U0.ri[j] = Ap->UU.U0.ri[j - subres.NumChavesPromovidas];
                Ap->UU.U0.pi[j + 1] = Ap->UU.U0.pi[j + 1 - subres.NumChavesPromovidas];
            }

            // insere as chaves e filhos promovidos
            for (int k = 0; k < subres.NumChavesPromovidas; k++) {
                Ap->UU.U0.ri[i + k] = subres.ChavesPromovidas[k];
                Ap->UU.U0.pi[i + k] = subres.FilhosResultantes[k];
            }
            Ap->UU.U0.pi[i + subres.NumChavesPromovidas] = subres.FilhosResultantes[subres.NumChavesPromovidas];
            Ap->UU.U0.ni += subres.NumChavesPromovidas;

            resultado.Cresceu = FALSE;
            return resultado;
        }

        
        // Nó interno cheio - precisa dividir
        TipoChave tempChaves[mm + 2];
        TipoApontador tempPtrs[mm + 3];
        int nTemp = 0;

        // Copia chaves e ponteiros do nó atual para vetores temporários
        for (int j = 0; j < Ap->UU.U0.ni; j++) {
            tempChaves[nTemp] = Ap->UU.U0.ri[j];
            tempPtrs[nTemp] = Ap->UU.U0.pi[j];
            nTemp++;
        }
        tempPtrs[nTemp] = Ap->UU.U0.pi[Ap->UU.U0.ni];

        // Insere chaves promovidas na posição correta
        for (int k = 0; k < subres.NumChavesPromovidas; k++) {
            // Localiza posição correta para inserção
            int pos = 0;
            while (pos < nTemp && subres.ChavesPromovidas[k] > tempChaves[pos]) pos++;

            // Move para abrir espaço
            for (int j = nTemp; j > pos; j--) {
                tempChaves[j] = tempChaves[j - 1];
                tempPtrs[j + 1] = tempPtrs[j];
            }

            tempChaves[pos] = subres.ChavesPromovidas[k];
            tempPtrs[pos] = subres.FilhosResultantes[k];
            tempPtrs[pos + 1] = subres.FilhosResultantes[k + 1];
            nTemp++;
        }

        // Divide o vetor em dois nós
        int meio = nTemp / 2;

        TipoApontador novoNo = CriaPagina(Interna, mm, mm2);

        // Reconstrói o nó original
        Ap->UU.U0.ni = 0;
        for (int j = 0; j < meio; j++) {
            Ap->UU.U0.ri[j] = tempChaves[j];
            Ap->UU.U0.pi[j] = tempPtrs[j];
            Ap->UU.U0.ni++;
        }
        Ap->UU.U0.pi[meio] = tempPtrs[meio];

        // Preenche o novo nó
        novoNo->UU.U0.ni = 0;
        for (int j = meio + 1; j < nTemp; j++) {
            novoNo->UU.U0.ri[novoNo->UU.U0.ni] = tempChaves[j];
            novoNo->UU.U0.pi[novoNo->UU.U0.ni] = tempPtrs[j];
            novoNo->UU.U0.ni++;
        }
        novoNo->UU.U0.pi[novoNo->UU.U0.ni] = tempPtrs[nTemp];

        // Prepara promoção
        resultado.Cresceu = TRUE;
        resultado.NumChavesPromovidas = 1;
        resultado.ChavesPromovidas[0] = tempChaves[meio];
        resultado.FilhosResultantes[0] = Ap;
        resultado.FilhosResultantes[1] = novoNo;
        return resultado;

    } else { // Nó folha
        int i = 0;
        while (i < Ap->UU.U1.ne && Reg.Chave > Ap->UU.U1.re[i].Chave)
            i++;

        // Verifica duplicata
        if (i < Ap->UU.U1.ne && Reg.Chave == Ap->UU.U1.re[i].Chave) {
            printf("Erro: Chave já existe na folha!\n");
            resultado.Cresceu = FALSE;
            return resultado;
        }

        // Folha tem espaço
        if (Ap->UU.U1.ne < mm2) {
            // Verifica posição válida
            if (i < 0 || i > Ap->UU.U1.ne) {
                printf("Erro: Posição de inserção inválida!\n");
                resultado.Cresceu = FALSE;
                return resultado;
            }

            // Faz inserção ordenada
            for (int j = Ap->UU.U1.ne; j > i; j--)
                Ap->UU.U1.re[j] = Ap->UU.U1.re[j - 1];
            
            Ap->UU.U1.re[i] = Reg;
            Ap->UU.U1.ne++;
            
            // Atualiza chave no pai se necessário
            if (i == 0 && Pai != NULL && PosFilho > 0) {
                Pai->UU.U0.ri[PosFilho-1] = Ap->UU.U1.re[0].Chave;
            }
            
            resultado.Cresceu = FALSE;
            return resultado;
        }

        // Folha cheia - caso especial para raiz
        if (Pai == NULL) {
            TipoApontador novaFolha = CriaPagina(Externa, mm, mm2);
            TipoRegistro temp[mm2*2];
            int nTemp = 0;

            // Copia registros existentes + novo registro
            for (int j = 0; j < i; j++)
                temp[nTemp++] = Ap->UU.U1.re[j];
            
            temp[nTemp++] = Reg;
            
            for (int j = i; j < Ap->UU.U1.ne; j++)
                temp[nTemp++] = Ap->UU.U1.re[j];

            // Divide exatamente no meio
            int meio = nTemp / 2;
            Ap->UU.U1.ne = 0;
            for (int j = 0; j < meio; j++)
                Ap->UU.U1.re[Ap->UU.U1.ne++] = temp[j];

            novaFolha->UU.U1.ne = 0;
            for (int j = meio; j < nTemp; j++)
                novaFolha->UU.U1.re[novaFolha->UU.U1.ne++] = temp[j];

            // Cria nova raiz
            TipoApontador novaRaiz = CriaPagina(Interna, mm, mm2);
            novaRaiz->UU.U0.ri[0] = novaFolha->UU.U1.re[0].Chave;
            novaRaiz->UU.U0.pi[0] = Ap;
            novaRaiz->UU.U0.pi[1] = novaFolha;
            novaRaiz->UU.U0.ni = 1;

            resultado.Cresceu = TRUE;
            resultado.NumChavesPromovidas = 1;
            resultado.ChavesPromovidas[0] = novaFolha->UU.U1.re[0].Chave;
            resultado.FilhosResultantes[0] = Ap;
            resultado.FilhosResultantes[1] = novaFolha;
            return resultado;
        }

        // Tenta redistribuição com irmãos
        TipoApontador IrmaoEsq = (PosFilho > 0) ? Pai->UU.U0.pi[PosFilho - 1] : NULL;
        TipoApontador IrmaoDir = (PosFilho < Pai->UU.U0.ni) ? Pai->UU.U0.pi[PosFilho + 1] : NULL;

        // Redistribuição com irmão esquerdo
        if (IrmaoEsq && IrmaoEsq->UU.U1.ne < mm2) {
            TipoRegistro temp[mm2 * 2];
            int k = 0;

            // Junta todos os registros
            for (int j = 0; j < IrmaoEsq->UU.U1.ne; j++) 
                temp[k++] = IrmaoEsq->UU.U1.re[j];
            for (int j = 0; j < Ap->UU.U1.ne; j++) 
                temp[k++] = Ap->UU.U1.re[j];
            
            // Insere o novo registro na posição correta
            for (int j = k; j > i + IrmaoEsq->UU.U1.ne; j--)
                temp[j] = temp[j - 1];
            temp[i + IrmaoEsq->UU.U1.ne] = Reg;
            k++;

            // Ordena
            for (int x = 0; x < k - 1; x++) {
                for (int y = x + 1; y < k; y++) {
                    if (temp[x].Chave > temp[y].Chave) {
                        TipoRegistro aux = temp[x];
                        temp[x] = temp[y];
                        temp[y] = aux;
                    }
                }
            }

            // Redivide entre os nós
            int novoMeio = k / 2;
            IrmaoEsq->UU.U1.ne = 0;
            for (int j = 0; j < novoMeio; j++)
                IrmaoEsq->UU.U1.re[IrmaoEsq->UU.U1.ne++] = temp[j];

            Ap->UU.U1.ne = 0;
            for (int j = novoMeio; j < k; j++)
                Ap->UU.U1.re[Ap->UU.U1.ne++] = temp[j];

            // Atualiza chaves no pai
            Pai->UU.U0.ri[PosFilho - 1] = Ap->UU.U1.re[0].Chave;
            resultado.Cresceu = FALSE;
            return resultado;
        }

        // Redistribuição com irmão direito
        if (IrmaoDir && IrmaoDir->UU.U1.ne < mm2) {
            TipoRegistro temp[mm2 * 2];
            int k = 0;

            for (int j = 0; j < Ap->UU.U1.ne; j++) 
                temp[k++] = Ap->UU.U1.re[j];
            for (int j = 0; j < IrmaoDir->UU.U1.ne; j++) 
                temp[k++] = IrmaoDir->UU.U1.re[j];
            
            // Insere novo registro
            for (int j = k; j > i; j--)
                temp[j] = temp[j - 1];
            temp[i] = Reg;
            k++;

            // Ordena
            for (int x = 0; x < k - 1; x++) {
                for (int y = x + 1; y < k; y++) {
                    if (temp[x].Chave > temp[y].Chave) {
                        TipoRegistro aux = temp[x];
                        temp[x] = temp[y];
                        temp[y] = aux;
                    }
                }
            }

            // Redivide
            int novoMeio = k / 2;
            Ap->UU.U1.ne = 0;
            for (int j = 0; j < novoMeio; j++)
                Ap->UU.U1.re[Ap->UU.U1.ne++] = temp[j];

            IrmaoDir->UU.U1.ne = 0;
            for (int j = novoMeio; j < k; j++)
                IrmaoDir->UU.U1.re[IrmaoDir->UU.U1.ne++] = temp[j];

            // Atualiza chaves no pai
            Pai->UU.U0.ri[PosFilho] = IrmaoDir->UU.U1.re[0].Chave;
            resultado.Cresceu = FALSE;
            return resultado;
        }

        // Não foi possível redistribuir - faz divisão 2-para-3
        TipoRegistro temp[mm2 * 2 + 1];
        int nTemp = 0;

        // Copia registros existentes
        for (int j = 0; j < Ap->UU.U1.ne; j++)
            temp[nTemp++] = Ap->UU.U1.re[j];

        // Insere novo registro na posição correta
        for (int j = nTemp; j > i; j--)
            temp[j] = temp[j - 1];
        temp[i] = Reg;
        nTemp++;

        // Divide em três partes
        int partSize = nTemp / 3;
        int resto = nTemp % 3;
        TipoApontador f2 = CriaPagina(Externa, mm, mm2);
        TipoApontador f3 = CriaPagina(Externa, mm, mm2);

        // Preenche o nó original
        Ap->UU.U1.ne = 0;
        for (int j = 0; j < partSize + (resto > 0); j++)
            Ap->UU.U1.re[Ap->UU.U1.ne++] = temp[j];

        // Preenche o segundo nó
        f2->UU.U1.ne = 0;
        for (int j = partSize + (resto > 0); j < 2 * partSize + (resto > 1); j++)
            f2->UU.U1.re[f2->UU.U1.ne++] = temp[j];

        // Preenche o terceiro nó
        f3->UU.U1.ne = 0;
        for (int j = 2 * partSize + (resto > 1); j < nTemp; j++)
            f3->UU.U1.re[f3->UU.U1.ne++] = temp[j];

        // Prepara resultado
        resultado.Cresceu = TRUE;
        resultado.NumChavesPromovidas = 2;
        resultado.ChavesPromovidas[0] = f2->UU.U1.re[0].Chave;
        resultado.ChavesPromovidas[1] = f3->UU.U1.re[0].Chave;
        resultado.FilhosResultantes[0] = Ap;
        resultado.FilhosResultantes[1] = f2;
        resultado.FilhosResultantes[2] = f3;
        return resultado;
    }
}


void ImprimeArvore(TipoApontador Ap, int nivel) {
    if (Ap == NULL) return;

    for (int i = 0; i < nivel; i++) printf("   ");
    if (Ap->Pt == Interna) {
        printf("[Interna] ");
        for (int i = 0; i < Ap->UU.U0.ni; i++) {
            printf("%ld ", Ap->UU.U0.ri[i]);
        }
        printf("\n");
        for (int i = 0; i <= Ap->UU.U0.ni; i++) {
            ImprimeArvore(Ap->UU.U0.pi[i], nivel + 1);
        }
    } else {
        printf("[Folha] ");
        for (int i = 0; i < Ap->UU.U1.ne; i++) {
            printf("%ld ", Ap->UU.U1.re[i].Chave);
        }
        printf("\n");
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


void Insere(TipoRegistro Reg, TipoApontador *Ap, int mm, int mm2) {
    // CASO 1: A ÁRVORE ESTÁ VAZIA — CRIA PRIMEIRA FOLHA
    if (*Ap == NULL) {
        TipoApontador novaFolha = CriaPagina(Externa, mm, mm2);
        novaFolha->UU.U1.re[0] = Reg;
        novaFolha->UU.U1.ne = 1;
        *Ap = novaFolha;
        return;
    }

    // CASO 2: ÁRVORE EXISTE — INICIA A INSERÇÃO RECURSIVA
    ResultadoIns resultado = Ins(Reg, *Ap, NULL, 0, mm, mm2);

    if (resultado.Cresceu) {
        TipoApontador novaRaiz = CriaPagina(Interna, mm, mm2);

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
        free(arvore->UU.U0.ri);
        free(arvore->UU.U0.pi);
    }else
        free(arvore->UU.U1.re);
    free(arvore); // Libera a própria página
}