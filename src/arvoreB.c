#include <stdio.h>
#include <string.h> // Adicionado para strcpy, usado implicitamente em algumas cópias de struct
#include <stdlib.h> // Para malloc, free
#include <stdbool.h> // Para o tipo bool

// Includes para o contexto do seu trabalho
#include "include/arvoreB.h"       // Seu cabeçalho do método
#include "include/common_types.h" // Para TipoChave e TipoRegistro
#include "include/utils.h"        // Para os contadores

// As structs item_arq e pagina serão definidas no arvoreB.h
// A constante ORDEM será definida no arvoreB.h

// Função para inicializar a árvore (renomeada e ajustada para Pagina**)
void inicializa_arvoreB(Pagina** arvore){ // MODIFICADO: Aceita Pagina**
	*arvore = NULL; // Correto para inicializar a raiz fora da função.
}

// Função de pesquisa (renomeada)
bool pesquisa_arvoreB(Item_arq* procurado, Pagina* pagina){
	long int i = 0; // MODIFICADO: Começando 'i' em 0 para 0-based indexing

	if(pagina == NULL)
		return false;

    incrementar_io(); // Contagem de I/O para acesso à página

	//procurando qual o intervalo correto (0-based)
	// Encontrar o menor 'i' tal que procurado->Chave <= pagina->registro[i].Chave
	for(i = 0; i < pagina->n; i++) // Loop até < pagina->n
	{
		incrementar_comparacao(); // Contagem de comparação
		if(procurado->Chave <= pagina->registro[i].Chave) // MODIFICADO: usar <= e registro[i]
			break; // Se menor ou igual, paramos. 'i' é o índice do possível item ou subárvore.
	}

	// Após o loop, 'i' é o índice onde a chave 'procurado->Chave' *deveria* estar
    // ou o índice da subárvore para descer.

	// Verifica se a chave foi encontrada (se 'i' é um índice válido E a chave é igual)
	if(i < pagina->n && procurado->Chave == pagina->registro[i].Chave) // MODIFICADO: usar 'i' e 'i < pagina->n'
	{
		incrementar_comparacao(); // Última comparação para igualdade
		*procurado = pagina->registro[i]; // MODIFICADO: usar registro[i]
		return true; // Retorna true
	}
    // Se não foi encontrada, desce para a subárvore apropriada
    // 'i' é o índice do apontador para a próxima página.
    // Se a chave não foi encontrada e 'i' == pagina->n, significa que a chave é maior que todos
    // os itens na página, então desce pelo último apontador (pagina->apontador[pagina->n]).
    // Caso contrário, desce pelo apontador[i].

    return pesquisa_arvoreB(procurado, pagina->apontador[i]); // MODIFICADO: sempre pagina->apontador[i]
}
// Função para inserir um item em uma página (mantida original, com TipoRegistro e contadores)
void insere_na_pagina(Pagina* pag_atual, Item_arq reg, Pagina* pont_dir){
	bool nao_achou_poisc; // MODIFICADO: de int para bool
	int k = pag_atual->n;
	nao_achou_poisc = (k > 0);

	while(nao_achou_poisc)
	{
		incrementar_comparacao(); // Contagem de comparação
		if(reg.Chave >= pag_atual->registro[k - 1].Chave) // MODIFICADO: usar .Chave
		{
			nao_achou_poisc = false;
			break;
		}

		pag_atual->registro[k] = pag_atual->registro[k - 1];
		pag_atual->apontador[k + 1] = pag_atual->apontador[k];
		k = k - 1;
		if(k < 1)
			break;
	}

	pag_atual->registro[k] = reg;
	pag_atual->apontador[k+1] = pont_dir;
	pag_atual->n = pag_atual->n + 1;
}

// Função de inserção recursiva (renomeada, ajustada para bool* e Pagina**)
void ins_arvoreB(Item_arq reg, Pagina* pag_atual, bool* cresceu, Item_arq* reg_retorno, Pagina** pont_retorno){ // MODIFICADO: int* cresceu -> bool* cresceu; Pagina* pont_retorno -> Pagina** pont_retorno
	long int i = 1;
	long int j;

	Pagina* pont_temporario = NULL;

	// verifica se a pagina atual realmente existe
	if(pag_atual == NULL)
	{
		*cresceu = true; // MODIFICADO: true em vez de 1
		(*reg_retorno) = reg;
		*pont_retorno = NULL; // MODIFICADO: *pont_retorno para atribuir corretamente
		return;
	}

	incrementar_io(); // Contagem de I/O: acesso à página
	
	//encontrando aonde seria a posicao para o reg
	while((i < pag_atual->n) && (reg.Chave > pag_atual->registro[i - 1].Chave)) // MODIFICADO: usar .Chave
		i++;
	incrementar_comparacao(); // Contagem de comparação (para a última comparação do loop ou condição)


	// verificando se o reg ja nao existe
	if(reg.Chave == pag_atual->registro[i - 1].Chave) // MODIFICADO: usar .Chave
	{
		*cresceu = false; // MODIFICADO: false em vez de 0
		return;
	}

	if(reg.Chave < pag_atual->registro[i - 1].Chave) // MODIFICADO: usar .Chave
		i = i - 1;

	ins_arvoreB(reg, pag_atual->apontador[i], cresceu, reg_retorno, pont_retorno);

	// se cresceu = false, significa que o reg ja se encontra na arvore, ou que conseguimos inserir o reg em uma pagina sem problemas
	if(!(*cresceu)) // MODIFICADO: usar !(*cresceu)
		return;

	
	// verifica se ha espaco na pagina atual
	if(pag_atual->n < (ORDEM * 2))
	{
		insere_na_pagina(pag_atual, *reg_retorno, *pont_retorno); // MODIFICADO: usar *pont_retorno
		*cresceu = false; // MODIFICADO: false em vez de 0
		return;
	}

	// criando uma nova pagina
	pont_temporario = malloc(sizeof(Pagina));
	// Não há verificação de NULL no original para este malloc, mantido assim.
	pont_temporario->n = 0;
	pont_temporario->apontador[0] = NULL;

	// verificando se o reg sera inserido na pag atual ou na nova pag
	if(i < (ORDEM + 1))
	{
		// transferindo o ultimo item da pag atual para a nova pag
		insere_na_pagina(pont_temporario, pag_atual->registro[(2 * ORDEM) - 1], pag_atual->apontador[2 * ORDEM]);
		pag_atual->n = pag_atual->n - 1;

		//inserindo reg na pag atual
		insere_na_pagina(pag_atual, *reg_retorno, *pont_retorno); // MODIFICADO: usar *pont_retorno
	}
	else // inserindo reg na nova pag
		insere_na_pagina(pont_temporario, *reg_retorno, *pont_retorno); // MODIFICADO: usar *pont_retorno

	
	// transferindo todos os da segunda metade da pag atual para a nova pag
	for( j = ORDEM + 2; j <= 2 * ORDEM; j++)
		insere_na_pagina(pont_temporario, pag_atual->registro[j - 1], pag_atual->apontador[j]);

	pag_atual->n = ORDEM;
	pont_temporario->apontador[0] = pag_atual->apontador[ORDEM + 1];
	*reg_retorno = pag_atual->registro[ORDEM];
	*pont_retorno = pont_temporario; // MODIFICADO: *pont_retorno para atribuir corretamente
	*cresceu = true; // MODIFICADO: true em vez de 1
	incrementar_io(); // Contagem de I/O: para a nova página criada pelo split.
}

// Função de inserção (renomeada, ajustada para Pagina**)
void insere_arvoreB(Item_arq reg, Pagina** arvore){ // MODIFICADO: Aceita Pagina**
	bool cresceu; // MODIFICADO: de int para bool
	Item_arq reg_retorno;
	Pagina* pont_retorno; // Ponteiro simples aqui, seu endereço é passado para ins_arvoreB
	Pagina* pont_temporario;

	ins_arvoreB(reg, *arvore, &cresceu, &reg_retorno, &pont_retorno); // MODIFICADO: passa &pont_retorno

	// se cresceu = true, significa que houve divisao na raiz, e a arvore aumentara de tamanho, tendo que criar uma nova raiz
	if(cresceu) // MODIFICADO: usar bool
	{
		pont_temporario = malloc(sizeof(Pagina));
		// Não há verificação de NULL no original para este malloc, mantido assim.
		pont_temporario->n = 1;
		pont_temporario->registro[0] = reg_retorno;
		pont_temporario->apontador[1] = pont_retorno;
		pont_temporario->apontador[0] = *arvore;
		*arvore = pont_temporario; // MODIFICADO: *arvore para atualizar a raiz corretamente.
	}
}

// Função para liberar a memória da árvore B
void libera_arvoreB(Pagina* pagina) {
    if (pagina == NULL) return;

    for (int i = 0; i <= pagina->n; i++) {
        libera_arvoreB(pagina->apontador[i]);
    }
    free(pagina);
}