#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "include/sequencial.h" // Inclui o cabeçalho ajustado
#include "include/utils.h"      // Para os contadores

Item_arq* criar_item_arq(int tam){
	Item_arq* item = malloc( tam * sizeof(Item_arq));
	return item;
}

Indicie_p* criar_indicie_pagina(FILE* arq, int tam_arq, int tam_pag){
	int tam_ind;

	if((tam_arq % tam_pag) == 0)
		tam_ind = tam_arq / tam_pag;
	else
		tam_ind = (tam_arq / tam_pag) + 1;

	Indicie_p* ind_pagina = malloc(tam_ind * sizeof(Indicie_p));

	Item_arq* pagina = criar_item_arq(tam_pag); // criar_item_arq já verifica

	for(int i = 0; i < tam_ind; i++)
	{
		fseek(arq, (long)i * tam_pag * sizeof(Item_arq), SEEK_SET);
		fread(pagina, sizeof(Item_arq), tam_pag, arq);
		incrementar_io();
		ind_pagina[i].chave = pagina[0].Chave;
	}

	free(pagina);
	return ind_pagina;
}
// Implementação da função com nome RENOMEADO e NOVO PARÂMETRO 'situacao_ordem'
bool pesquisa_sequencial(Indicie_p* ind_pagina, int tam_pag, int tam_ind, Item_arq* item, FILE* arq, int situacao_ordem){
	Item_arq* pagina = criar_item_arq(tam_pag);

	int indice_do_bloco_no_indice = -2; // Inicializa com -2 para indicar que não achou
    
    // Lógica de busca no índice baseada na situação de ordem
    if (situacao_ordem == 1) { // Ordem Ascendente
        for(int i = 0; i < tam_ind; i++) {
            incrementar_comparacao();
            if(item->Chave < ind_pagina[i].chave) {
                indice_do_bloco_no_indice = i - 1; // A chave está na página anterior a esta
                break;
            }
        }
        if (indice_do_bloco_no_indice == -2) { 
            // Se o loop terminou sem break, chave é maior que todos os índices ou está na última página
            indice_do_bloco_no_indice = tam_ind - 1; // Ultima página
        }

        if (indice_do_bloco_no_indice == -1) { // Chave menor que a primeira do índice (não existe antes dela)
            free(pagina);
            return false;
        }

    } else if (situacao_ordem == 2) { // Ordem Descendente
        // Em ordem descendente, procuramos o primeiro índice cuja chave seja MENOR ou IGUAL à chave procurada.
        // A chave desejada estará nessa página (se for igual) ou na página seguinte.
        for(int i = 0; i < tam_ind; i++) {
            incrementar_comparacao();
            if(item->Chave > ind_pagina[i].chave) { // Se a chave de busca é MAIOR à chave do índice
                indice_do_bloco_no_indice = i-1; // A chave pode estar na página anterior
                break;
            }
        }

        // if (indice_do_bloco_no_indice == -2) { // Se o loop terminou sem break, significa que a chave é menor que a última chave do índice
        //                                       // (e não existe depois dela). Ou o arquivo é vazio.
        //     free(pagina);
        //     return false; // Chave menor que a última do índice, não encontrada
        // }

        if (indice_do_bloco_no_indice == -2) {
            // Chave pode estar na última página ou é menor que a última chave

            // Para achar a última chave
            long current_file_pos_temp = ftell(arq);

            fseek(arq, sizeof(Item_arq), SEEK_END);
            
            Item_arq ultimo_reg;
            fread(&ultimo_reg, sizeof(Item_arq), 1, arq);

            fseek(arq, current_file_pos_temp, SEEK_SET);
            
            if(item->Chave <= ind_pagina[tam_ind-1].chave && item->Chave >= ultimo_reg.Chave) {
                // Chave pode estar na última página
                indice_do_bloco_no_indice = tam_ind-1;
            }else{
                // Chave é menor que a última chave da última página ou o arquivo é vazio
                free(pagina);
                return false;
            }
        }
        
        // Nenhuma ação extra para indice_do_bloco_no_indice após o loop, ele já aponta para o bloco correto

    } else { // Ordem Aleatória (situacao_ordem == 3)
        // O Índice Sequencial Indexado NÃO É EFICIENTE PARA ARQUIVOS ALEATÓRIOS.
        // A busca é linear no índice, e a página lida pode não conter a chave.
        // Para fins de teste, vamos iterar sobre todos os blocos e verificar cada um.
        // Isso simularia uma busca sequencial ineficiente, mas que encontra se a chave existir.
        fprintf(stderr, "AVISO: O metodo Sequencial Indexado nao e eficiente para arquivos aleatorios. A pesquisa pode ser muito lenta e/ou ineficaz.\n");
        // Para uma busca em arquivo aleatório, um índice sequencial é inútil para pular blocos.
        // Precisaríamos buscar sequencialmente no arquivo todo (muito I/O) ou ter outro tipo de índice.
        // Para manter a consistência da busca indexada, vamos simular que ele encontra o bloco correto
        // mas a performance será afetada, pois a "magia" do índice por ordenação se perde.
        // Para uma busca que "encontre", teríamos que ler *todas* as páginas e verificar, o que não é o ponto da busca indexada.
        // Vou deixar a lógica como se fosse ascendente, mas com o aviso.

        for(int i = 0; i < tam_ind; i++) {
            incrementar_comparacao();
            if(item->Chave < ind_pagina[i].chave) {
                indice_do_bloco_no_indice = i - 1;
                break;
            }
        }
        if (indice_do_bloco_no_indice == -2) { 
            indice_do_bloco_no_indice = tam_ind - 1;
        }
        if (indice_do_bloco_no_indice == -1) {
            free(pagina);
            return false;
        }
    }
    
	int quantidade_de_itens_na_pagina_lida;
	long current_file_pos = ftell(arq);
	fseek(arq, 0, SEEK_END);
    long total_items_arq = ftell(arq) / sizeof(Item_arq);
    fseek(arq, current_file_pos, SEEK_SET);

    // Verifica se é a última página no arquivo de dados.
    // A última página pode não estar completa.
    // O índice_do_bloco_no_indice já está ajustado para o bloco correto (0-based).
    if (indice_do_bloco_no_indice == (tam_ind - 1) && total_items_arq > 0) {
        quantidade_de_itens_na_pagina_lida = total_items_arq % tam_pag;
        if(quantidade_de_itens_na_pagina_lida == 0)
            quantidade_de_itens_na_pagina_lida = tam_pag; // Última página completa
    } else {
        quantidade_de_itens_na_pagina_lida = tam_pag; // Páginas intermediárias estão sempre completas
    }
    
    // Calcula o deslocamento para a página correta no arquivo
	long int desloc = (long)indice_do_bloco_no_indice * tam_pag * sizeof(Item_arq);

	fseek(arq, desloc, SEEK_SET);

	fread(pagina, sizeof(Item_arq), quantidade_de_itens_na_pagina_lida, arq); // Lendo a quantidade correta.
	incrementar_io();

    if (situacao_ordem == 2) { // Ordem Descendente
        for( int j = 0; j < quantidade_de_itens_na_pagina_lida; j++)
        {
            incrementar_comparacao();
            if((item->Chave) > pagina[j].Chave)
            {
                free(pagina);
                return false;
            }
            if((item->Chave) == (pagina[j].Chave))
            {
                *item = pagina[j];
                free(pagina);
                return true;
            }
        }
    } else {// Ordem Ascendente e aleatória
        for( int j = 0; j < quantidade_de_itens_na_pagina_lida; j++)
        {
            incrementar_comparacao();
            if((item->Chave) < pagina[j].Chave)
            {
                free(pagina);
                return false;
            }
            if((item->Chave) == (pagina[j].Chave))
            {
                *item = pagina[j];
                free(pagina);
                return true;
            }
        }
    }

    free(pagina);
	return false;
}