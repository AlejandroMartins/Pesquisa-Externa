// src/dataGenerator.c
// para compilar: gcc -o dataGenerator.exe dataGenerator.c src/utils.c -Isrc/include -std=c99
// para executar ./dataGenerator.exe

#include <stdio.h>   // Para FILE, fopen, fclose, printf, perror
#include <stdlib.h>  // Para malloc, free, rand, srand, qsort, exit, EXIT_FAILURE
#include <time.h>    // Para time (para srand)
#include <string.h>  // Para sprintf, memset

#include "./src/include/common_types.h" // Inclui TipoRegistro e TipoChave

// Função para gerar um registro aleatório com uma chave específica
TipoRegistro gerarRegistroAleatorio(TipoChave chave) {
    TipoRegistro reg;
    reg.Chave = chave;
    reg.dado1 = rand() * 1000L; // Gera um long int aleatório
    // Preenche dado2 com caracteres aleatórios e termina com '\0' para ter 1000 caracteres
    memset(reg.dado2, 'A' + (rand() % 26), 999);
    reg.dado2[999] = '\0'; // Garante terminação de string
    // Preenche dado3 com caracteres aleatórios e termina com '\0' para ter 5000 caracteres
    memset(reg.dado3, 'A' + (rand() % 26), 4999);
    reg.dado3[4999] = '\0'; // Garante terminação de string
    return reg;
}

// Comparador para qsort: ordem ascendente de chaves
int compararAscendente(const void *a, const void *b) {
    TipoRegistro *reg_a = (TipoRegistro *)a;
    TipoRegistro *reg_b = (TipoRegistro *)b;
    if (reg_a->Chave < reg_b->Chave) return -1;
    if (reg_a->Chave > reg_b->Chave) return 1;
    return 0;
}

// Comparador para qsort: ordem descendente de chaves
int compararDescendente(const void *a, const void *b) {
    TipoRegistro *reg_a = (TipoRegistro *)a;
    TipoRegistro *reg_b = (TipoRegistro *)b;
    if (reg_a->Chave > reg_b->Chave) return -1;
    if (reg_a->Chave < reg_b->Chave) return 1;
    return 0;
}

/**
 * @brief Gera um arquivo binário de registros com a quantidade e ordem especificadas.
 * @param filename O nome do arquivo a ser criado (incluindo o caminho da pasta de saída).
 * @param quantidade O número de registros a serem gerados.
 * @param situacao A situação de ordem (1: ascendente, 2: descendente, 3: aleatória). 
 */
void gerarArquivoDados(const char *filename, long quantidade, int situacao) {
    FILE *file = fopen(filename, "wb"); // Cria o arquivo em modo binário de escrita
    if (file == NULL) {
        perror("Erro ao criar o arquivo de dados"); // Reporta erro de abertura
        return;
    }

    // Aloca memória para armazenar todos os registros temporariamente
    TipoRegistro *registros = (TipoRegistro *)malloc(quantidade * sizeof(TipoRegistro));
    if (registros == NULL) {
        perror("Erro de alocacao de memoria para registros");
        fclose(file); // Fecha o arquivo se a alocação falhar
        return;
    }

    srand(time(NULL)); // Inicializa o gerador de números aleatórios com o tempo atual

    // Geração inicial dos registros com chaves sequenciais (para facilitar a ordenação)
    for (long i = 0; i < quantidade; i++) {
        registros[i] = gerarRegistroAleatorio(i + 1); // Chaves de 1 a 'quantidade'
    }

    // Aplica a ordenação conforme a situação especificada
    if (situacao == 1) { // Ordem ascendente
        qsort(registros, quantidade, sizeof(TipoRegistro), compararAscendente);
    } else if (situacao == 2) { // Ordem descendente
        qsort(registros, quantidade, sizeof(TipoRegistro), compararDescendente);
    } else { // Ordem aleatória
        // Para uma ordem realmente aleatória das chaves, primeiro geramos chaves aleatórias
        for (long i = 0; i < quantidade; i++) {
            // Gera chaves aleatórias em um range um pouco maior para reduzir colisões
            registros[i].Chave = (TipoChave)(rand() % (quantidade * 5) + 1);
        }
        // Em seguida, embaralha o array para garantir a desordem aleatória dos registros completos
        for (long i = 0; i < quantidade - 1; i++) {
            long j = i + (rand() % (quantidade - i)); // Escolhe um índice aleatório à frente
            TipoRegistro temp = registros[i]; // Troca os registros
            registros[i] = registros[j];
            registros[j] = temp;
        }
        // Depois de embaralhar as chaves e a ordem, re-gera os dados de string para as novas chaves
        // Isso garante que dado2 e dado3 correspondam à chave final do registro.
        for (long i = 0; i < quantidade; i++) {
            memset(registros[i].dado2, 'A' + (rand() % 26), 999);
            registros[i].dado2[999] = '\0';
            memset(registros[i].dado3, 'A' + (rand() % 26), 4999);
            registros[i].dado3[4999] = '\0';
        }
    }

    // Escreve todos os registros (ordenados ou aleatórios) no arquivo binário
    fwrite(registros, sizeof(TipoRegistro), quantidade, file);

    free(registros); // Libera a memória alocada para os registros
    fclose(file);    // Fecha o arquivo
    printf("Arquivo '%s' gerado com %ld registros na situacao %d.\n", filename, quantidade, situacao);
}

int main(int argc, char *argv[]) {
    printf("--- Geracao de Arquivos de Dados para o TP01 ---\n");

    // Arrays predefinidos de quantidades de registros e situações de ordem
    long quantidades[] = {100, 1000, 10000, 100000, 1000000};
    int situacoes[] = {1, 2, 3}; // 1: ascendente, 2: descendente, 3: aleatoria 
    char filename[256]; // Buffer para armazenar o nome do arquivo

    // Loops aninhados para gerar arquivos para todas as combinações de quantidade e situação
    for (int q = 0; q < sizeof(quantidades) / sizeof(quantidades[0]); q++) {
        for (int s = 0; s < sizeof(situacoes) / sizeof(situacoes[0]); s++) {
            // Determina a string correspondente à situação de ordem
            const char* situacao_str;
            if (situacoes[s] == 1) situacao_str = "asc";
            else if (situacoes[s] == 2) situacao_str = "desc";
            else situacao_str = "rand";

            // Cria o nome do arquivo, direcionando-o para a pasta 'data/'
            sprintf(filename, "data/dados_%ld_%s.bin", quantidades[q], situacao_str);
            // Chama a função para gerar o arquivo de dados
            gerarArquivoDados(filename, quantidades[q], situacoes[s]);
        }
    }

    printf("\nGeracao de todos os arquivos de dados concluida.\n");
    return 0; // Retorna 0 para indicar sucesso
}