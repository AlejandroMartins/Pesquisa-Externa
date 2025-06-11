#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "arvoreB.h"

#define ORDEM 2

struct item_arq{
	long int chave;
	long int dado1;
	char dado2[1000], dado3[5000];
};

struct pagina{
	int n; //numero de registros dentro dessa pagina
	Item_arq registro[2 * ORDEM];
	Pagina* apontador[(2 * ORDEM) + 1];
};

void inicializa(Pagina* arvore){
	arvore = NULL;
}

bool pesquisa(Item_arq* procurado, Pagina* pagina){
	long int i = 1;

	if(pagina == NULL)
		return false;

	//procurando qual o intervalo correto
	for(; i <= pagina->n; i++)
	{
		if(procurado->chave < pagina->registro[i - 1].chave)
			break;
	}

	//verifica se ja foi encontrada a chave
	if(procurado->chave == pagina->registro[i - 1].chave)
	{
		*procurado = pagina->registro[i - 1];
		return true;
	}

	if(procurado->chave < pagina->registro[i - 1].chave)
		pesquisa(procurado, pagina->apontador[i - 1]);
	else if(procurado->chave > pagina->registro[i - 1].chave)
		pesquisa(procurado, pagina->apontador[i]);
}

void insere_na_pagina(Pagina* pag_atual, Item_arq reg, Pagina* pont_dir){
	bool nao_achou_poisc;
	int k = pag_atual->n;
	nao_achou_poisc = (k > 0);

	while(nao_achou_poisc)
	{
		if(reg.chave >= pag_atual->registro[k - 1].chave)
		{
			nao_achou_poisc = false;
			break;
		}

		pag_atual->registro[k] = pag_atual->registro[k - 1]; // arredando o resgistro para o lado
		pag_atual->apontador[k + 1] = pag_atual->apontador[k]; // arredando o ponteiro da pagina para o lado
		k = k - 1;
		if(k < 1)
			break;
	}

	pag_atual->registro[k] = reg;
	pag_atual->apontador[k+1] = pont_dir;
	pag_atual->n = pag_atual->n + 1;
}


void ins(Item_arq reg, Pagina* pag_atual, int* cresceu, Item_arq* reg_retorno, Pagina* pont_retorno){
	long int i = 1;
	long int j;

	Pagina* pont_temporario = NULL;

	// verifica se a pagina atual realmente existe
	if(pag_atual == NULL)
	{
		*cresceu = 1;
		(*reg_retorno) = reg;
		pont_retorno = NULL;
		return;
	}

	//encontrando aonde seria a posicao para o reg
	while((i < pag_atual->n) && (reg.chave > pag_atual->registro[i - 1].chave))
		i++;


	// verificando se o reg ja nao existe
	if(reg.chave == pag_atual->registro[i - 1].chave)
	{
		printf("ERRO: esse registro ja está presente na arvore.\n");
		*cresceu = 0;
		return;
	}

	if(reg.chave < pag_atual->registro[i - 1].chave)
		i = i - 1;

	ins(reg, pag_atual->apontador[i], cresceu, reg_retorno, pont_retorno);

	// se cresceu = 0, significa que o reg ja se encontra na arvore, ou que conseguimos inserir o reg em uma pagina sem problemas
	if((*cresceu) == 0)
		return;

	
	// verifica se ha espaco na pagina atual
	if(pag_atual->n < (ORDEM * 2))
	{
		insere_na_pagina(pag_atual, *reg_retorno, pont_retorno);
		*cresceu = 0;
		return;
	}

	// criando uma nova pagina
	pont_temporario = malloc(sizeof(Pagina));
	pont_temporario->n = 0;
	pont_temporario->apontador[0] = NULL;

	// verificando se o reg sera inserido na pag atual ou na nova pag
	if(i < (ORDEM + 1))
	{
		// transferindo o ultimo item da pag atual para a nova pag
		insere_na_pagina(pont_temporario, pag_atual->registro[(2 * ORDEM) - 1], pag_atual->apontador[2 * ORDEM]);
		pag_atual->n = pag_atual->n - 1;

		//inserindo reg na pag atual
		insere_na_pagina(pag_atual, *reg_retorno, pont_retorno);
	}
	else // inserindo reg na nova pag
		insere_na_pagina(pont_temporario, *reg_retorno, pont_retorno);

	
	// transferindo todos os da segunda metade da pag atual para a nova pag
	for( j = ORDEM + 2; j <= 2 * ORDEM; j++)
		insere_na_pagina(pont_temporario, pag_atual->registro[j - 1], pag_atual->apontador[j]);

	pag_atual->n = ORDEM;
	pont_temporario->apontador[0] = pag_atual->apontador[ORDEM + 1];
	*reg_retorno = pag_atual->registro[ORDEM];
	pont_retorno = pont_temporario;
	
}

void insere(Item_arq reg, Pagina* arvore){
	int cresceu;
	Item_arq reg_retorno;
	Pagina* pont_retorno;
	Pagina*pont_temporario;
	ins(reg, arvore, &cresceu, &reg_retorno, pont_retorno);

	// se cresceu = 1, significa que houve divisao na raiz, e a ravore aumentara de tamanho, tendo que criar uma nova raiz
	if(cresceu)
	{
		pont_temporario = malloc(sizeof(Pagina));
		pont_temporario->n = 1;
		pont_temporario->registro[0] = reg_retorno;
		pont_temporario->apontador[1] = pont_retorno;
		pont_temporario->apontador[0] = arvore;
		arvore = pont_temporario;

	}
}