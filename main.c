// main.c
// Este arquivo deve ficar na pasta raiz do projeto (TP1/)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Inclui os cabeçalhos comuns primeiro
#include "src/include/common_types.h"
#include "src/include/utils.h"

// Inclui os cabeçalhos de todos os métodos (agora com nomes RENOMEADOS nos .h)
#include "src/include/sequencial.h" // Renomeou pesquisa -> pesquisa_sequencial
#include "src/include/arvBin.h"
#include "src/include/arvoreB.h"     // Renomeou varias funções
#include "src/include/arvoreEstrela.h"


int main(int argc, char *argv[]) {
    int metodo_escolhido;
    long quantidade_registros;
    int situacao_ordem;
    TipoChave chave_pesquisa;
    bool exibir_chaves_debug = false;

    char filename[256];
    FILE *arquivo_dados = NULL;

    clock_t inicio_tempo_geral;
    double tempo_execucao_pesquisa;
    double tempo_execucao_construcao_indice;


    if (argc < 5) {
        fprintf(stderr, "Uso: %s <metodo> <quantidade> <situacao> <chave> [-P]\n", argv[0]);
        fprintf(stderr, "  <metodo>: 1=Sequencial Indexado, 2=Arvore Binaria Externa, 3=Arvore B, 4=Arvore B*\n");
        fprintf(stderr, "  <quantidade>: 100, 1000, 10000, 100000, 1000000\n");
        fprintf(stderr, "  <situacao>: 1=Ascendente, 2=Descendente, 3=Aleatoria\n");
        fprintf(stderr, "  <chave>: Chave a ser pesquisada\n");
        fprintf(stderr, "  [-P]: Opcional, exibe chaves dos registros (debug)\n");
        return 1;
    }

    metodo_escolhido = atoi(argv[1]);
    quantidade_registros = atol(argv[2]);
    situacao_ordem = atoi(argv[3]);
    chave_pesquisa = atol(argv[4]);

    if (argc == 6 && strcmp(argv[5], "-P") == 0) {
        exibir_chaves_debug = true;
    }

    if (metodo_escolhido < 1 || metodo_escolhido > 4) {
        fprintf(stderr, "Erro: Metodo invalido. Escolha entre 1 e 4.\n");
        return 1;
    }
    if (quantidade_registros != 100 && quantidade_registros != 1000 &&
        quantidade_registros != 10000 && quantidade_registros != 100000 &&
        quantidade_registros != 1000000) {
        fprintf(stderr, "Erro: Quantidade de registros invalida. Escolha 100, 1000, 10000, 100000 ou 1000000.\n");
        return 1;
    }
    if (situacao_ordem < 1 || situacao_ordem > 3) {
        fprintf(stderr, "Erro: Situacao de ordem invalida. Escolha 1 (Ascendente), 2 (Descendente) ou 3 (Aleatoria).\n");
        return 1;
    }

    const char* situacao_str;
    if (situacao_ordem == 1) situacao_str = "asc";
    else if (situacao_ordem == 2) situacao_str = "desc";
    else situacao_str = "rand";
    
    sprintf(filename, "data/dados_%ld_%s.bin", quantidade_registros, situacao_str);

    arquivo_dados = fopen(filename, "rb");
    if (arquivo_dados == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo de dados '%s'. Certifique-se de que ele foi gerado.\n", filename);
        return 1;
    }

    printf("--- Iniciando Pesquisa ---\n");
    printf("Metodo: %d, Quantidade: %ld, Situacao: %d, Chave: %ld\n",
           metodo_escolhido, quantidade_registros, situacao_ordem, chave_pesquisa);
    printf("Arquivo de dados: %s\n", filename);

    TipoRegistro registro_encontrado;
    registro_encontrado.Chave = chave_pesquisa;
    bool encontrado = false;

    switch (metodo_escolhido) {
        case 1: { // 1: Acesso Sequencial Indexado
            int tam_pagina_sequencial = 4;
            
            fseek(arquivo_dados, 0, SEEK_END);
            long tam_arq_bytes = ftell(arquivo_dados);
            fseek(arquivo_dados, 0, SEEK_SET);

            int tam_arq_itens = tam_arq_bytes / sizeof(TipoRegistro);
            
            resetar_contadores();
            inicio_tempo_geral = iniciar_tempo();
            Indicie_p* indice_sequencial = criar_indicie_pagina(arquivo_dados, tam_arq_itens, tam_pagina_sequencial);
            tempo_execucao_construcao_indice = finalizar_tempo(inicio_tempo_geral);

            printf("\n--- Criacao do Indice (Sequencial Indexado) ---\n");
            printf("Tempo de construcao do indice: %.6f segundos\n", tempo_execucao_construcao_indice);
            printf("Transferencias (I/O) na construcao do indice: %ld\n", g_io_transferencias);
            printf("Comparacoes na construcao do indice: %ld\n", g_comparacoes_chaves);

            resetar_contadores();
            inicio_tempo_geral = iniciar_tempo();

            // CHAMADA CORRIGIDA: usa pesquisa_sequencial (nome renomeado) E PASSA situacao_ordem
            encontrado = pesquisa_sequencial(indice_sequencial, tam_pagina_sequencial, (tam_arq_itens + tam_pagina_sequencial - 1) / tam_pagina_sequencial, &registro_encontrado, arquivo_dados, situacao_ordem);
            
            tempo_execucao_pesquisa = finalizar_tempo(inicio_tempo_geral);
            free(indice_sequencial);
            break;
        }

        case 2: { // 2: Árvore Binária de Pesquisa adequada à memória externa
            FILE* arv_bin_file = fopen("arvore_binaria_externa.dat", "r+b");
            if (!arv_bin_file) {
                arv_bin_file = fopen("arvore_binaria_externa.dat", "w+b");
                if (!arv_bin_file) {
                    perror("Erro ao criar/abrir arquivo da Arvore Binaria Externa");
                    fclose(arquivo_dados);
                    return 1;
                }
                printf("\n--- Construcao da Arvore Binaria Externa (em disco) ---\n");
                resetar_contadores();
                inicio_tempo_geral = iniciar_tempo();
                
                TipoRegistro temp_reg;
                long pos_raiz_bin_arv = -1;
                fseek(arquivo_dados, 0, SEEK_SET);
                while(fread(&temp_reg, sizeof(TipoRegistro), 1, arquivo_dados) == 1) {
                    incrementar_io();
                    pos_raiz_bin_arv = inserir(arv_bin_file, temp_reg, pos_raiz_bin_arv);
                }
                tempo_execucao_construcao_indice = finalizar_tempo(inicio_tempo_geral);
                printf("Tempo de construcao do indice: %.6f segundos\n", tempo_execucao_construcao_indice);
                printf("Transferencias (I/O) na construcao do indice: %ld\n", g_io_transferencias);
                printf("Comparacoes na construcao do indice: %ld\n", g_comparacoes_chaves);
            } else {
                printf("\n--- Carregando Arvore Binaria Externa existente ---\n");
                tempo_execucao_construcao_indice = 0;
                long file_size = 0;
                fseek(arv_bin_file, 0, SEEK_END);
                file_size = ftell(arv_bin_file);
                fseek(arv_bin_file, 0, SEEK_SET);
                if (file_size == 0) {
                    fprintf(stderr, "Erro: Arquivo 'arvore_binaria_externa.dat' vazio ou corrompido. Tente apagar o arquivo e executar novamente.\n");
                    fclose(arv_bin_file);
                    fclose(arquivo_dados);
                    return 1;
                }
            }

            resetar_contadores();
            inicio_tempo_geral = iniciar_tempo();

            encontrado = buscar(arv_bin_file, chave_pesquisa, 0, &registro_encontrado);
            
            tempo_execucao_pesquisa = finalizar_tempo(inicio_tempo_geral);
            fclose(arv_bin_file);
            break;
        }

        case 3: { // 3: Árvore B
            Pagina* arvoreB_raiz = NULL;
            // CHAMADA CORRIGIDA: usa inicializa_arvoreB (nome renomeado) e passa &arvoreB_raiz
            inicializa_arvoreB(&arvoreB_raiz); 

            printf("\n--- Criacao do Indice (Arvore B) ---\n");
            resetar_contadores();
            inicio_tempo_geral = iniciar_tempo();
            
            TipoRegistro temp_reg_b;
            fseek(arquivo_dados, 0, SEEK_SET);
            while(fread(&temp_reg_b, sizeof(TipoRegistro), 1, arquivo_dados) == 1) {
                incrementar_io();
                // CHAMADA CORRIGIDA: usa insere_arvoreB (nome renomeado) e passa &arvoreB_raiz
                insere_arvoreB(temp_reg_b, &arvoreB_raiz);
            }
            tempo_execucao_construcao_indice = finalizar_tempo(inicio_tempo_geral);
            printf("Tempo de construcao do indice: %.6f segundos\n", tempo_execucao_construcao_indice);
            printf("Transferencias (I/O) na construcao do indice: %ld\n", g_io_transferencias);
            printf("Comparacoes na construcao do indice: %ld\n", g_comparacoes_chaves);

            resetar_contadores();
            inicio_tempo_geral = iniciar_tempo();

            // CHAMADA CORRIGIDA: usa pesquisa_arvoreB (nome renomeado)
            encontrado = pesquisa_arvoreB(&registro_encontrado, arvoreB_raiz);
            
            tempo_execucao_pesquisa = finalizar_tempo(inicio_tempo_geral);
            libera_arvoreB(arvoreB_raiz);
            break;
        }

        case 4: { // 4: Árvore B*
            TipoApontador arvoreBStar_raiz = NULL;
            // CHAMADA ORIGINAL: Inicializa (nome original da B*)
            Inicializa(&arvoreBStar_raiz); 

            printf("\n--- Criacao do Indice (Arvore B*) ---\n");
            resetar_contadores();
            inicio_tempo_geral = iniciar_tempo();
            
            TipoRegistro temp_reg_bstar;
            fseek(arquivo_dados, 0, SEEK_SET);
            while(fread(&temp_reg_bstar, sizeof(TipoRegistro), 1, arquivo_dados) == 1) {
                incrementar_io();
                Insere(temp_reg_bstar, &arvoreBStar_raiz); // Insere espera TipoRegistro e TipoApontador*
            }
            tempo_execucao_construcao_indice = finalizar_tempo(inicio_tempo_geral);
            printf("Tempo de construcao do indice: %.6f segundos\n", tempo_execucao_construcao_indice);
            printf("Transferencias (I/O) na construcao do indice: %ld\n", g_io_transferencias);
            printf("Comparacoes na construcao do indice: %ld\n", g_comparacoes_chaves);

            resetar_contadores();
            inicio_tempo_geral = iniciar_tempo();

            Pesquisa(&registro_encontrado, &arvoreBStar_raiz);
            if (registro_encontrado.Chave == chave_pesquisa) {
                encontrado = true;
            } else {
                encontrado = false;
            }
            
            tempo_execucao_pesquisa = finalizar_tempo(inicio_tempo_geral);
            liberaArvoreBStar(arvoreBStar_raiz);
            break;
        }
    }

    printf("\n--- Resultados da Pesquisa da Chave %ld ---\n", chave_pesquisa);
    if (encontrado) {
        printf("Chave %ld: ENCONTRADA!\n", chave_pesquisa);
        printf("  Dado1: %ld\n", registro_encontrado.dado1);
        printf("  Dado2: %s\n", registro_encontrado.dado2);
        printf("  Dado3: %s\n", registro_encontrado.dado3);
    } else {
        printf("Chave %ld: NAO ENCONTRADA.\n", chave_pesquisa);
    }

    printf("Numero de transferencias (I/O) na pesquisa: %ld\n", g_io_transferencias);
    printf("Numero de comparacoes entre chaves na pesquisa: %ld\n", g_comparacoes_chaves);
    printf("Tempo de execucao da pesquisa: %.6f segundos\n", tempo_execucao_pesquisa);

    if (exibir_chaves_debug) {
        printf("\n--- Conteudo do Arquivo (apenas chaves para debug) ---\n");
        fseek(arquivo_dados, 0, SEEK_SET);
        TipoRegistro temp_reg;
        int count = 0;
        while (fread(&temp_reg, sizeof(TipoRegistro), 1, arquivo_dados) == 1) {
            printf("%ld ", temp_reg.Chave);
            count++;
            if (count % 10 == 0) printf("\n");
        }
        printf("\n");
    }

    fclose(arquivo_dados);

    return 0;
}