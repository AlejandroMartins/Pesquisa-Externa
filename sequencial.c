#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sequencial.h"

struct indicie_p{
	int chave;
};

struct item_arq{
	int chave;
	long int dado1;
	char dado2[1000], dado3[5000];
};


Item_arq* criar_item_arq(int tam){
	Item_arq* item = malloc( tam * sizeof(Item_arq));
	return item;
}

Indicie_p* criar_indicie_pagina(FILE* arq, int tam_arq, int tam_pag){
	int tam_ind;

	//definindo qual sera o tamanho do indicie de pagina
	if((tam_arq % tam_pag) == 0)
		tam_ind = tam_arq / tam_pag;
	else
		tam_ind = (tam_arq / tam_pag) + 1;

	//alocando o indicie de pagina
	Indicie_p* ind_pagina = malloc(tam_ind * sizeof(Indicie_p));

	// criando uma pagina para armazenar os itens do arquivo
	Item_arq* pagina = criar_item_arq(tam_pag);

	//colocando os primeiros itens de cada pagina do arquivo no indicie de paginas
	for(int i = 0; i < tam_ind; i++)
	{
		fread(pagina, sizeof(Item_arq), tam_pag, arq);
		ind_pagina[i].chave = pagina[0].chave;
	}

	free(pagina);
	return ind_pagina;
}


bool pesquisa(Indicie_p* ind_pagina, int tam_pag, int tam_ind, Item_arq* item, FILE* arq){
	//criando a pagina que recebera o item procurado
	Item_arq* pagina = criar_item_arq(tam_pag);

	//procurando a pagina que o item podera estar
	int i = 0;
	for(; i < tam_ind; i++)
	{
		//se a chave procurada for menor que a primeira chave da pagina i+1 do arquivo, paramos a execucao
		if((item->chave) < (ind_pagina[i].chave))
			break;
	}

	// se a chave procurada for menor que a primeira chave, entao o item nao esta no arquivo
	if(!i)
		return false;

	//caso a ultima pagina nao esteja completa e o item estiver nela, teremos que tratar essa questao
	int quantidade;
	if(i == tam_ind)
	{
		//caso o item esteja na ultima pagina, teremos que reduzir i em 1 unidade 
		i--;
		fseek(arq, 0, 2);
		quantidade = (ftell(arq) / sizeof(Item_arq)) % tam_pag;
		if(!quantidade)
			quantidade = tam_pag;
	}
	else
		quantidade = tam_pag;

	long int desloc = (i - 1) * tam_pag * sizeof(Item_arq);

	fseek(arq, desloc, 0);

	//lendo a pagina desejada do arquivo
	fread(pagina, sizeof(Item_arq), tam_pag, arq);

	//realisando a pesquisa na pagina em questao
	for( int i = 0; i < quantidade; i++)
	{
		if((item->chave) < pagina[i].chave)
			return false;
		if((item->chave) == (pagina[i].chave))
		{
			*item = pagina[i];
			return true;
		}
	}
	return false;



}
