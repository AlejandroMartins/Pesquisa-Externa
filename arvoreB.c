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

void reconstitui(Pagina* pag_atual, Pagina* pag_pai, int posic_pai, int* diminui){
	Pagina* pag_aux = NULL;
	long int disp_aux, j;

	if(posic_pai < pag_pai->n)
	{
		pag_aux = pag_pai->apontador[posic_pai + 1];
		disp_aux = (pag_aux->n - ORDEM + 1) / 2;
		pag_atual->registro[pag_atual->n] = pag_pai->registro[posic_pai];
		pag_atual->apontador[pag_atual->n + 1] = pag_aux->apontador[0];
		pag_atual->n = pag_atual->n + 1;

		if(disp_aux > 0) // existe folga, transfere de pag_aux para pag_atual
		{
			for(j = 1; j < disp_aux; j++)
				insere_na_pagina(pag_atual, pag_aux->registro[j - 1], pag_aux->apontador[j]); // transferindo de pag_aux para pag_atual
			pag_pai->registro[posic_pai] = pag_aux->registro[disp_aux - 1];
			pag_aux->n = pag_aux->n - disp_aux;

			for(j = 0; j < pag_aux->n; j++)
				pag_aux->registro[j] = pag_aux->registro[j + disp_aux];

			for(j = 0; j <= pag_aux->n; j++)
				pag_aux->apontador[j] = pag_aux->apontador[j + disp_aux];

			*diminui = 0;
		}
		else // junta pag_aux com pag_atual
		{
			for(j = 1; j <= ORDEM; j++)
				insere_na_pagina(pag_atual, pag_aux->registro[j - 1], pag_aux->apontador[j]);
			free(pag_aux);

			for(j = posic_pai + 1; j < pag_pai->n; j++)
			{
				pag_pai->registro[j - 1] = pag_pai->registro[j];
				pag_pai->apontador[j] = pag_pai->apontador[j + 1];
			}

			pag_pai->n = pag_pai->n - 1;
			if(pag_pai->n >= ORDEM)
				*diminui = 0;
		}
	}
	else
	{
		pag_aux = pag_pai->apontador[posic_pai - 1];
		disp_aux = (pag_aux->n - ORDEM + 1) / 2;

		for(j = pag_atual->n; j >= 1; j--)
			pag_atual->registro[j] = pag_atual->registro[j - 1];
		pag_atual->registro[0] = pag_pai->registro[posic_pai - 1];

		for(j = pag_atual->n; j >= 0; j--)
			pag_atual->n = pag_atual->n + 1;

		if(disp_aux > 0) // existe folga, transfere de pag_aux para pag_atual
		{
			for(j = 1; j < disp_aux; j++)
				insere_na_pagina(pag_atual, pag_aux->registro[pag_aux->n - j], pag_aux->apontador[pag_aux->n - j + 1]);

			pag_atual->apontador[0] = pag_aux->apontador[pag_aux->n - disp_aux + 1];
			pag_pai->registro[posic_pai - 1] = pag_aux->registro[pag_aux->n - disp_aux];
			pag_aux->n = pag_aux->n - disp_aux;
			*diminui = 0;
		}
		else // junta pag_atual com pag_aux
		{
			for(j = 1; j <= ORDEM; j++)
				insere_na_pagina(pag_aux, pag_atual->registro[j - 1], pag_atual->apontador[j]);
			free(pag_atual);
			pag_pai->n = pag_pai->n - 1;

			if(pag_pai->n >= ORDEM)
				*diminui = 0;
		}
	}
}

void antecessor(Pagina* pag_atual, int indicie, Pagina* pag_pai, int* diminui){
	if(pag_pai->apontador[pag_pai->n] != NULL)
	{
		antecessor(pag_atual, indicie, pag_pai->apontador[pag_pai->n], diminui);
		if(*diminui == 1)
			reconstitui(pag_pai->apontador[pag_pai->n], pag_pai, (long) pag_pai->n, diminui);
		return;
	}
	pag_atual->registro[indicie - 1] = pag_pai->registro[pag_pai->n - 1];
	pag_pai->n = pag_pai->n - 1;
	*diminui = (pag_pai->n < ORDEM);
}


void ret(long int chave, Pagina* pag_atual, int* diminui){
	long int j;
	long int indicie = 1;
	Pagina* pag_aux = NULL; 
	if(pag_atual == NULL)
	{
		printf("ERRO: o registro de chave %ld nao esta na arvore!\n", chave);
		*diminui = 0;
		return;
	}
	pag_aux = pag_atual;

	while((indicie < pag_aux->n) && (chave > pag_aux->registro[indicie - 1].chave)) // encontrando aode poderia estar o registro 
		indicie++;

	if(chave == pag_aux->registro[indicie - 1].chave) // se a chave foi encotrada na pag_aux
	{
		if(pag_aux->apontador[indicie - 1] == NULL) //se for pag_aux uma pagina folha
		{
			pag_aux->n = pag_aux->n - 1;
			*diminui = (pag_aux->n < ORDEM); // se a pagina ficar com menos registros que o permitido diminui tera valor verdadeiro
			for(j = indicie; j < pag_aux->n; j++) // arredando os registros e apontadores para o lado para ocupar o espaco do que sera retirado
			{
				pag_aux->registro[j - 1] = pag_aux->registro[j];
				pag_aux->apontador[j] = pag_aux->apontador[j + 1];
			}
			return;
		}
		// se pag_aux nao for pagina folha
		antecessor(pag_atual, indicie, pag_aux->apontador[indicie - 1], diminui); //encontrando o antecessor do registro que sera retirado

		if(*diminui == 1) //se alguma pagina foi violada
		{
			reconstitui(pag_aux->apontador[indicie - 1], pag_atual, indicie - 1, diminui); //reconstruindo a arvore
			return;
		}
	}
	// se a a chave nao se encontra na pag_aux

	if(chave > pag_aux->registro[indicie - 1].chave)
		indicie++;
	ret(chave, pag_aux->apontador[indicie - 1], diminui); // chamando novamente a funçao recursivamente

	if(*diminui == 1)
		reconstitui(pag_aux->apontador[indicie - 1], pag_atual, indicie - 1, diminui);
}

void retira(long int chave, Pagina* pag_atual){
	int diminui;
	Pagina* pag_aux = NULL;
	ret(chave, pag_atual, &diminui);

	if(diminui && (pag_atual->n == 0)) //arvore diminui na altura
	{
		pag_aux = pag_atual;
		pag_atual = pag_aux->apontador[0];
		free(pag_aux);
	}
}