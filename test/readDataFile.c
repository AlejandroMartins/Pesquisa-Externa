#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/common_types.h" // Para a definição de TipoRegistro

// executar: test/readDataFile.exe data/dados_1000_rand.bin

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nome_do_arquivo.bin>\n", argv[0]); // CORRIGIDO: argv0 -> argv[0]
        printf("Exemplo: %s data/dados_100_asc.bin\n", argv[0]); // CORRIGIDO: argv0 -> argv[0]
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    TipoRegistro reg;
    int count = 0;
    printf("Lendo registros de: %s\n", argv[1]);
    printf("----------------------------------\n");

    // Lendo os primeiros 10 registros
    printf("Primeiros 10 registros:\n");
    while (fread(&reg, sizeof(TipoRegistro), 1, file) == 1 && count < 10) {
        printf("Chave: %ld, Dado1: %ld, Dado2 (inicio): %.20s...\n", 
               reg.Chave, reg.dado1, reg.dado2);
        count++;
    }
    printf("----------------------------------\n");

    // Lendo os últimos 10 registros (se o arquivo for grande o suficiente)
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    long num_records = file_size / sizeof(TipoRegistro);

    if (num_records > 10) {
        printf("\nUltimos 10 registros (ou menos se o arquivo for pequeno):\n");
        // Pula para a posição dos últimos 10 registros
        fseek(file, (num_records - 10) * sizeof(TipoRegistro), SEEK_SET);
        count = 0;
        while (fread(&reg, sizeof(TipoRegistro), 1, file) == 1) {
            printf("Chave: %ld, Dado1: %ld, Dado2 (inicio): %.100s..., Dado3 (inicio): %.50s...\n", 
                   reg.Chave, reg.dado1, reg.dado2, reg.dado3);
            count++;
        }
    }
    printf("----------------------------------\n");

    fclose(file);
    return 0;
}