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


// -----------------------------------------------------------------------------
// Implementação das funções auxiliares (static) - Baseadas no PDF e adaptadas
// -----------------------------------------------------------------------------

/**
 * @brief Insere uma chave e um apontador em uma página interna (nó de índice).
 * @param Ap Apontador para a página interna onde inserir.
 * @param Ch A chave a ser inserida.
 * @param ApDir O apontador direito associado à chave.
 */
static void InsereNaPaginaInterna(TipoApontador Ap, TipoChave Ch, TipoApontador ApDir) {
    bool NaoAchouPosicao;
    int k;

    k = Ap->UU.U0.ni; // Número atual de chaves na página interna
    NaoAchouPosicao = (k > 0);

    // Percorre a página de trás para frente para encontrar a posição de inserção
    // e deslocar os itens maiores para a direita
    while (NaoAchouPosicao) {
        incrementar_comparacao(); // Conta a comparação
        if (Ch >= Ap->UU.U0.ri[k - 1]) { // Se a nova chave é maior ou igual, encontrou a posição
            NaoAchouPosicao = false;
            break;
        }
        // Desloca a chave e o apontador
        Ap->UU.U0.ri[k] = Ap->UU.U0.ri[k - 1];
        Ap->UU.U0.pi[k + 1] = Ap->UU.U0.pi[k];
        k--;
        if (k < 0) { // Se chegou antes da primeira posição (posição 0)
            NaoAchouPosicao = false;
        }
    }
    // Insere a nova chave e o apontador no local correto
    Ap->UU.U0.ri[k] = Ch;
    Ap->UU.U0.pi[k + 1] = ApDir;
    Ap->UU.U0.ni++; // Incrementa o contador de chaves na página
}

/**
 * @brief Insere um registro em uma página externa (folha).
 * @param Ap Apontador para a página folha onde inserir.
 * @param Reg O registro completo a ser inserido.
 */
static void InsereNaPaginaExterna(TipoApontador Ap, TipoRegistro Reg) {
    bool NaoAchouPosicao;
    int k;

    k = Ap->UU.U1.ne; // Número atual de registros na página folha
    NaoAchouPosicao = (k > 0);

    // Percorre a página de trás para frente para encontrar a posição de inserção
    // e deslocar os registros maiores para a direita
    while (NaoAchouPosicao) {
        incrementar_comparacao(); // Conta a comparação
        if (Reg.Chave >= Ap->UU.U1.re[k - 1].Chave) { // Se a chave do novo registro é maior ou igual
            NaoAchouPosicao = false;
            break;
        }
        // Desloca o registro
        Ap->UU.U1.re[k] = Ap->UU.U1.re[k - 1];
        k--;
        if (k < 0) { // Se chegou antes da primeira posição (posição 0)
            NaoAchouPosicao = false;
        }
    }
    // Insere o novo registro no local correto
    Ap->UU.U1.re[k] = Reg; // Copia o registro completo (struct copy)
    Ap->UU.U1.ne++; // Incrementa o contador de registros na página
}

/**
 * @brief Função recursiva de inserção para Árvore B*.
 * @param Reg O registro a ser inserido.
 * @param Ap Apontador para a página atual (na recursão).
 * @param Cresceu Flag booleana que indica se a árvore cresceu em altura.
 * @param RegRetorno Registro cuja chave será promovida ao nível superior (se houver split).
 * @param ApRetorno Apontador para a nova página criada (se houver split).
 */
static void Ins(TipoRegistro Reg, TipoApontador Ap, bool *Cresceu, TipoRegistro *RegRetorno, TipoApontador *ApRetorno) {
    int i = 1; // Índice para busca na página (1-based no PDF)
    TipoApontador ApTemp; // Apontador temporário para nova página em caso de split

    // CASO BASE DA RECURSÃO: Se Ap é NULL, chegamos ao ponto de inserção.
    // Isso significa que precisamos criar uma nova página folha.
    if (Ap == NULL) {
        *Cresceu = true; // A página "cresceu" (na verdade, uma nova foi criada)
        (*RegRetorno) = Reg; // O registro a ser retornado para o nível superior é o próprio registro
        (*ApRetorno) = NULL; // Não há subárvore à direita (ainda não houve split)
        return;
    }

    incrementar_io(); // Conta o acesso a esta página (leitura)

    // Se a página atual é INTERNA (nó de índice)
    if (Ap->Pt == Interna) {
        // Encontra a posição para descer na árvore
        while (i < Ap->UU.U0.ni && Reg.Chave > Ap->UU.U0.ri[i - 1]) { // Usa 1-based para 'i' e 'i-1' para array
            incrementar_comparacao(); // Conta a comparação
            i++;
        }
        // Uma comparação final para decidir o caminho (se o loop não percorrer tudo)
        if (Ap->UU.U0.ni > 0) incrementar_comparacao();

        // Verifica se a chave já existe no NÓ DE ÍNDICE (teoricamente não deveria parar, mas é um erro lógico)
        if (Reg.Chave == Ap->UU.U0.ri[i - 1]) { // Usa 'i-1'
            printf("Erro: Chave %ld ja presente no indice (arvore B*).\n", Reg.Chave);
            *Cresceu = false; // Não houve inserção válida, não cresceu
            return;
        }

        // Ajusta o índice para a subárvore correta (se a chave for menor)
        if (Reg.Chave < Ap->UU.U0.ri[i - 1]) { // Usa 'i-1'
            i--;
        }
        
        // Chamada recursiva para descer na árvore
        Ins(Reg, Ap->UU.U0.pi[i], Cresceu, RegRetorno, ApRetorno);

        // Se a chamada recursiva não resultou em um split, não há mais o que fazer
        if (!(*Cresceu)) return;

        // Se a página interna tem espaço
        if (Ap->UU.U0.ni < MM) {
            InsereNaPaginaInterna(Ap, RegRetorno->Chave, *ApRetorno); // Insere a chave promovida
            *Cresceu = false; // Não precisa propagar o crescimento
            return;
        }

        // Overflow na página interna: precisa dividir
        ApTemp = (TipoApontador)malloc(sizeof(TipoPagina));
        if (ApTemp == NULL) { perror("Erro: malloc falhou para nova pagina interna (split)"); exit(EXIT_FAILURE); }
        ApTemp->Pt = Interna;
        ApTemp->UU.U0.ni = 0;
        ApTemp->UU.U0.pi[0] = NULL; // O primeiro apontador da nova página é nulo inicialmente

        // Lógica de divisão de página interna (similar à Árvore B)
        // Os 2m itens atuais + 1 novo item = 2m+1 itens para distribuir.
        // O elemento do meio sobe para o pai, e os demais são distribuídos entre as duas novas páginas.
        TipoChave ChPromovida;
        
        // Crie um array temporário para armazenar todas as chaves e ponteiros
        TipoChave temp_ri[MM + 1];
        TipoApontador temp_pi[MM + 2];
        int k_temp = 0;

        // Copia chaves e apontadores existentes e o novo para o array temporário
        for (int j = 0; j < Ap->UU.U0.ni; j++) {
            if (k_temp == i) { // Posição para inserir a chave promovida e seu apontador
                temp_ri[k_temp] = RegRetorno->Chave;
                temp_pi[k_temp + 1] = *ApRetorno;
                k_temp++;
            }
            temp_ri[k_temp] = Ap->UU.U0.ri[j];
            temp_pi[k_temp] = Ap->UU.U0.pi[j];
            k_temp++;
        }
        if (k_temp == i) { // Se o novo item deve ser o último
            temp_ri[k_temp] = RegRetorno->Chave;
            temp_pi[k_temp + 1] = *ApRetorno;
        } else {
             temp_pi[k_temp] = Ap->UU.U0.pi[Ap->UU.U0.ni]; // Copia o último apontador se não foi pego no loop
        }

        // Define a chave a ser promovida (o elemento do meio dos 2m+1 itens)
        ChPromovida = temp_ri[MM / 2];
        (*RegRetorno) = (TipoRegistro){.Chave = ChPromovida}; // Apenas a chave é promovida

        // Redistribui as chaves e ponteiros: primeira metade para Ap, segunda metade para ApTemp
        Ap->UU.U0.ni = 0; // Limpa a página original
        ApTemp->UU.U0.ni = 0; // Limpa a nova página

        // Copia a primeira metade dos itens para a página original (Ap)
        for (int j = 0; j < MM / 2; j++) {
            Ap->UU.U0.ri[Ap->UU.U0.ni++] = temp_ri[j];
            Ap->UU.U0.pi[j] = temp_pi[j];
        }
        Ap->UU.U0.pi[MM/2] = temp_pi[MM/2]; // Apontador da chave que subiu

        // Copia a segunda metade dos itens para a nova página (ApTemp)
        for (int j = (MM / 2) + 1; j <= MM; j++) {
            ApTemp->UU.U0.ri[ApTemp->UU.U0.ni++] = temp_ri[j];
            ApTemp->UU.U0.pi[j - (MM/2 + 1)] = temp_pi[j];
        }
        ApTemp->UU.U0.pi[ApTemp->UU.U0.ni] = temp_pi[MM + 1]; // Último apontador

        *ApRetorno = ApTemp; // Retorna o apontador para a nova página
        *Cresceu = true; // Continua propagando o crescimento
        incrementar_io(); // Contagem de I/O para a escrita da nova página criada pelo split.
        return;

    } else { // Se a página atual é EXTERNA (folha)
        // Encontra a posição para inserir o registro na folha
        int idx_folha_ins = 0; // Usar 0-based indexing para inserção
        while (idx_folha_ins < Ap->UU.U1.ne && Reg.Chave > Ap->UU.U1.re[idx_folha_ins].Chave) {
            incrementar_comparacao(); // Conta a comparação
            idx_folha_ins++;
        }
        // Uma comparação final para decidir o caminho (se o loop não percorrer tudo)
        if (Ap->UU.U1.ne > 0) incrementar_comparacao();

        // Verifica se a chave já existe na página folha
        if (idx_folha_ins < Ap->UU.U1.ne && Reg.Chave == Ap->UU.U1.re[idx_folha_ins].Chave) {
            printf("Erro: Registro com chave %ld ja esta presente na pagina folha (arvore B*).\n", Reg.Chave);
            *Cresceu = false; // Não houve inserção válida
            return;
        }

        // Se a página folha tem espaço
        if (Ap->UU.U1.ne < MM2) {
            InsereNaPaginaExterna(Ap, Reg); // Insere o registro na folha
            *Cresceu = false; // Não precisa propagar o crescimento
            return;
        }

        // Overflow na página folha: precisa dividir
        ApTemp = (TipoApontador)malloc(sizeof(TipoPagina));
        if (ApTemp == NULL) { perror("Erro: malloc falhou para nova pagina folha (split)"); exit(EXIT_FAILURE); }
        ApTemp->Pt = Externa;
        ApTemp->UU.U1.ne = 0;

        // Lógica de divisão de página folha (diferente da Árvore B padrão)
        // O item do meio é COPIADO para a página pai e o item original permanece na folha da direita.
        TipoRegistro RegMeio;
        
        // Crie um array temporário para armazenar todos os itens (MM2 existentes + 1 novo)
        TipoRegistro temp_registros[MM2 + 1];
        int k_temp = 0;

        // Copia os registros existentes e o novo para o array temporário
        for (int j = 0; j < Ap->UU.U1.ne; j++) {
            if (k_temp == idx_folha_ins) { // Posição para inserir o novo registro
                temp_registros[k_temp++] = Reg;
            }
            temp_registros[k_temp++] = Ap->UU.U1.re[j];
        }
        if (k_temp == idx_folha_ins) { // Se o novo registro deve ser o último
            temp_registros[k_temp++] = Reg;
        }

        // Reseta os contadores de itens das páginas
        Ap->UU.U1.ne = 0;
        ApTemp->UU.U1.ne = 0;

        // Define o registro do meio (o primeiro elemento da segunda metade)
        RegMeio = temp_registros[(MM2 + 1) / 2];

        // Distribui os itens: primeira metade para Ap, segunda metade para ApTemp
        for (int j = 0; j < (MM2 + 1) / 2; j++) {
            Ap->UU.U1.re[Ap->UU.U1.ne++] = temp_registros[j];
        }
        for (int j = (MM2 + 1) / 2; j < MM2 + 1; j++) {
            ApTemp->UU.U1.re[ApTemp->UU.U1.ne++] = temp_registros[j];
        }

        (*RegRetorno) = (TipoRegistro){.Chave = RegMeio.Chave}; // Apenas a chave é promovida para o pai
        *ApRetorno = ApTemp; // Retorna o apontador para a nova página
        *Cresceu = true; // Propaga o crescimento
        incrementar_io(); // Contagem de I/O para a escrita da nova página criada pelo split.
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

/**
 * @brief Insere um novo registro na árvore B*.
 * @param Reg O registro a ser inserido.
 * @param Ap Ponteiro para o apontador da raiz da árvore.
 */
void Insere(TipoRegistro Reg, TipoApontador *Ap) {
    bool Cresceu;
    TipoRegistro RegRetorno;
    TipoApontador ApRetorno = NULL; // Apontador para nova página em caso de split
    
    // Chamada à função recursiva de inserção
    Ins(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno);

    // Se a árvore cresceu em altura pela raiz (split da raiz)
    if (Cresceu) {
        TipoApontador ApTemp = (TipoApontador)malloc(sizeof(TipoPagina));
        if (ApTemp == NULL) { perror("Erro: malloc falhou para nova raiz B*"); exit(EXIT_FAILURE); }
        ApTemp->Pt = Interna; // A nova raiz é uma página interna
        ApTemp->UU.U0.ni = 1; // Ela terá um item (o que foi promovido)
        ApTemp->UU.U0.ri[0] = RegRetorno.Chave; // A chave promovida se torna o primeiro item
        ApTemp->UU.U0.pi[0] = *Ap; // O apontador esquerdo aponta para a antiga raiz
        ApTemp->UU.U0.pi[1] = ApRetorno; // O apontador direito aponta para a nova página criada
        *Ap = ApTemp; // Atualiza a raiz da árvore
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