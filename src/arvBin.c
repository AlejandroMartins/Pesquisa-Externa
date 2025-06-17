#include <stdio.h>
#include <stdlib.h> // Para malloc, free, exit, EXIT_FAILURE
#include <stdbool.h>
#include <string.h> // Para strcpy

// Includes para o contexto do seu trabalho
#include "include/arvBin.h"         // Seu cabeçalho do método
#include "include/common_types.h"   // Para TipoChave e TipoRegistro
#include "include/utils.h"          // Para os contadores

// A struct no_arv não deve ser redefinida aqui, ela vem do .h

// Função para criar um novo nó (MODIFICADO: chave agora é TipoChave e aceita TipoRegistro)
No_arv* criar_no(TipoChave chave, TipoRegistro dados) { // MODIFICADO: TipoChave chave, TipoRegistro dados
    No_arv* novo = malloc(sizeof(No_arv));
    if (novo == NULL) { // ADICIONADO: Verificação de alocação de memória
        perror("Erro: malloc falhou em criar_no");
        exit(EXIT_FAILURE);
    }
    novo->chave = chave;
    novo->esquerda = -1;   // -1 indica nenhum filho
    novo->direita = -1;
    // ADICIONADO: Cópia dos dados do TipoRegistro para os campos do No_arv
    novo->dado1 = dados.dado1;
    strcpy(novo->dado2, dados.dado2);
    strcpy(novo->dado3, dados.dado3);
    return novo;
}

// Função para escrever um nó no arquivo (ADICIONADO: contador)
void escrever_no(FILE* arquivo, No_arv* no, long pos) {
    if (arquivo == NULL || no == NULL) 
        return;
    
    fseek(arquivo, pos, SEEK_SET);
    fwrite(no, sizeof(No_arv), 1, arquivo);
    incrementar_io(); // ADICIONADO: Contagem de I/O para escrita do nó
}

// Função para ler um nó do arquivo (ADICIONADO: contador)
No_arv* ler_no(FILE* arquivo, long pos) {
    if (arquivo == NULL) 
        return NULL;
    
    No_arv* no = malloc(sizeof(No_arv));
    if (no == NULL) { // ADICIONADO: Verificação de alocação de memória
        perror("Erro: malloc falhou em ler_no");
        exit(EXIT_FAILURE);
    }
    fseek(arquivo, pos, SEEK_SET);
    fread(no, sizeof(No_arv), 1, arquivo);
    incrementar_io(); // ADICIONADO: Contagem de I/O para leitura do nó
    return no;
}

// Função para inserir uma chave na árvore (MODIFICADO: chave agora é TipoRegistro; ADICIONADO: contador)
long inserir(FILE* arquivo, TipoRegistro reg, long pos_atual) { // MODIFICADO: int chave -> TipoRegistro reg
    if (pos_atual == -1) 
    {
        // Criar novo nó com todos os dados do registro e escrevê-lo no final
        No_arv* novo = criar_no(reg.Chave, reg); // Passa TipoChave e TipoRegistro
        fseek(arquivo, 0, SEEK_END);
        long nova_pos = ftell(arquivo);
        escrever_no(arquivo, novo, nova_pos);
        free(novo);
        return nova_pos;
    }
    
    No_arv* atual = ler_no(arquivo, pos_atual);
    if (atual == NULL) { // ADICIONADO: Verificação de nó lido
        return -1; // Retorna erro se o nó não pôde ser lido
    }
    
    incrementar_comparacao(); // ADICIONADO: Contagem de comparação inicial
    if (reg.Chave < atual->chave) 
    {
        atual->esquerda = inserir(arquivo, reg, atual->esquerda);
        escrever_no(arquivo, atual, pos_atual);
    } 
    else if (reg.Chave > atual->chave) 
    {
        atual->direita = inserir(arquivo, reg, atual->direita);
        escrever_no(arquivo, atual, pos_atual);
    }
    // else: chave == atual->chave, não faz nada (seu código original não tratava duplicatas)
    
    free(atual);
    return pos_atual;
}

// Função para buscar uma chave na árvore (MODIFICADO: chave agora é TipoChave e retorna TipoRegistro*; ADICIONADO: contador)
bool buscar(FILE* arquivo, TipoChave chave, long pos_atual, TipoRegistro* out_reg_encontrado) { // MODIFICADO: No_arv* no -> TipoRegistro* out_reg_encontrado
    if (pos_atual == -1) 
        return false;
    
    No_arv* atual = ler_no(arquivo, pos_atual);
    if (atual == NULL) { // ADICIONADO: Verificação de nó lido
        return false;
    }
    
    incrementar_comparacao(); // ADICIONADO: Contagem de comparação
    bool encontrado = false;
    
    if (chave == atual->chave) 
    {
        encontrado = true;
        // Copia os dados do No_arv lido para o TipoRegistro de saída
        if (out_reg_encontrado != NULL) {
            out_reg_encontrado->Chave = atual->chave;
            out_reg_encontrado->dado1 = atual->dado1;
            strcpy(out_reg_encontrado->dado2, atual->dado2);
            strcpy(out_reg_encontrado->dado3, atual->dado3);
        }
    } 
    else if (chave < atual->chave) 
    {
        encontrado = buscar(arquivo, chave, atual->esquerda, out_reg_encontrado);
    } 
    else 
    {
        encontrado = buscar(arquivo, chave, atual->direita, out_reg_encontrado);
    }
    
    free(atual);
    return encontrado;
}