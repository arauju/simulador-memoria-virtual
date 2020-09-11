#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "memoriaVirtual.h"
#include "tabelaHash.h"
#include "Lista.h"

//char *algoritmo, *arquivo;
char linha[20];
char *algoritmo, *arquivo;
int tam_pag, tam_mem, num_pag, operacoes = 0, leituras = 0, escritas = 0;
int acertoPag = 0, faltaPag = 0, writebacks = 0, pag_usadas = 0, endereco_tmp;
float faults = 0;
int posicao = 0;
Lista *lista;
TabelaHash *memoriaVirtual;
FILE *file;
Pagina *primeiro, *ultimo;
int erro = 1;
bool escrita;

void procuraPagina(int endereco_tmp){
		bool encontrada = false;
		posicao = abs(posicao);
		Item *pag = memoriaVirtual->itens[posicao];

		if (memoriaVirtual->itens[posicao]->prox == NULL){
			//adiciona a pagina
			if (pag_usadas < num_pag){
				encontrada = true;
				Pagina *nova  = (Pagina*) calloc(1, sizeof(Pagina));
				dadosPag *dados;

				dados = (dadosPag*) malloc(sizeof(dadosPag));
				dados->offset = endereco_tmp;
				dados->posicao = posicao;
				if(strcmp(algoritmo, "segunda_chance") == 0)
					dados->bitR = 1;
				if(strcmp(algoritmo, "nru") == 0)
						dados->bitR = 0;	
				if (escrita == true)
					dados->bitM = 1;

				nova->offset = endereco_tmp;
				pag->prox = novoItem(&endereco_tmp, nova);
				faltaPag ++;
				faults ++;
				pag_usadas++;
				erro = insere_lista_final(lista, dados);
				if (erro == 0)
					exit(1);
			} else {
				substitui_pagina(endereco_tmp);
			}
		} else {
			/*Procura a pagina*/
			while (pag->prox != NULL && encontrada == false){
				if (pag->prox->valor->offset == endereco_tmp){
					/*Pagina encontrada*/
					pag->prox->valor->dirty = 1;
					/*Se o algoritmo for o nru, seta o bit R igual a 1 na lista*/
					if(strcmp(algoritmo, "nru") == 0){
						Elem *no = (Elem*) malloc(sizeof(Elem));
						no = *lista;
						no = no->prox;
						while (no != NULL && pag->prox->valor->offset != no->dados->offset)
							no = no->prox;
							if(no != NULL)
								no->dados->bitR = 1;
					}
					if(strcmp(algoritmo, "lru") == 0){
						/*Se o algoritmo for o lru move a pagina encontrada pro final da lista*/
						Elem *no = (Elem*) malloc(sizeof(Elem));
						no = *lista;
						no = no->prox;
						Elem *anterior = (Elem*) malloc(sizeof(Elem));
						bool achou = false;
						while (no != NULL && achou == false){
								if (pag->prox->valor->offset == no->dados->offset && no->prox != NULL){
									dadosPag *dadosAux;
									dadosAux = (dadosPag*) malloc(sizeof(dadosPag));
									dadosAux->posicao = no->dados->posicao;
									dadosAux->offset = no->dados->offset;
									insere_lista_final(lista, dadosAux);
									achou = true;

									anterior = no->ant;
									if(anterior != NULL)
										anterior->prox = no->prox;

									break;
								}
								no = no->prox;
							}
					}
					encontrada = true;
					acertoPag ++;
				}
				pag = pag->prox;
			}

			if (encontrada == false){
				//adiciona a pagina
				if (pag_usadas < num_pag){
					pag = memoriaVirtual->itens[posicao];

					while (pag->prox != NULL)
						pag = pag->prox;

					Pagina *nova  = (Pagina*) calloc(1, sizeof(Pagina));
					nova->offset = endereco_tmp;
					nova->paginaAlterada = 0;

					dadosPag *dados;
					dados = (dadosPag*) malloc(sizeof(dadosPag));

					dados->offset = endereco_tmp;
					dados->posicao = posicao;
					dados->bitR = 1;
					if (escrita == true)
						dados->bitM = 1;

					pag->prox = novoItem(&endereco_tmp, nova);
					pag_usadas ++;
					erro = insere_lista_final(lista, dados);
					if (erro == 0)
						exit(1);
				} else {
					substitui_pagina(endereco_tmp);
				}
				faltaPag ++;
				faults ++;
			}
		}
}

int segunda_chance (int endereco_tmp){
	bool removeu = false;
	Elem *no = (Elem*) malloc(sizeof(Elem));
	no = *lista;

	if (no->prox != NULL)
		*lista = no->prox;
	else
		exit(2);

	if(no == NULL)
		return 1;

	while (no != NULL && no->dados->bitR != 0){
		/*Retira a pagina do inicio da lista e adiciona ao final, mudando seu bit R*/
		dadosPag *dadosAux;
		dadosAux = (dadosPag*) malloc(sizeof(dadosPag));

		dadosAux->posicao = no->dados->posicao;
		dadosAux->offset = no->dados->offset;
		dadosAux->bitR = 0;
		no = no->prox;
		erro = insere_lista_final(lista, dadosAux);
		if (erro == 0)
			exit(1);
		erro = remove_lista_inicio(lista);
		if (erro == 0)
			exit(1);
	}

	/*Achou na lista quem tem bit R = 0*/
	if(no->dados->bitR == 0){
		/* Procura na tabela o correspondente*/
		Item *pag = memoriaVirtual->itens[no->dados->posicao];
		Item *anterior = pag;
		while (pag->prox != NULL && removeu == false){
			if (pag->prox->valor->offset == no->dados->offset){
				/*Remove da tabela*/
				if (pag->prox->valor->dirty == 1)
					writebacks++;
				pag = pag->prox;
				anterior->prox  = pag->prox;
				removeu = true;
				//no = no->prox;
				*lista = no->prox;
				//erro = remove_lista_inicio(lista);
				pag_usadas --;
			}
			if (removeu == false){
				anterior = pag;
				pag = pag->prox;
			}
		}
	}
	if (removeu == true){
		if (erro == 0)
			exit(1);
		/*Adiciona a página*/
		Pagina *nova  = (Pagina*) calloc(1, sizeof(Pagina));
		Item *pag = memoriaVirtual->itens[posicao];
		dadosPag *dados;
		dados = (dadosPag*) malloc(sizeof(dadosPag));

		while (pag->prox != NULL)
			pag = pag->prox;

		nova->offset = endereco_tmp;
		dados->offset = endereco_tmp;
		dados->posicao = posicao;
		dados->bitR = 1;

		pag->prox = novoItem(&endereco_tmp, nova);
		pag_usadas ++;
		erro = insere_lista_final(lista, dados);
		if (erro == 0)
			exit(1);
	}
	return 0;
}

void NRU (int endereco_tmp){
	Elem *no = (Elem*) malloc(sizeof(Elem));
	no = *lista;
	bool removeu = false;
	bool classe0 = false, classe1 = false, classe2 = false, classe3 = false;

	Elem *noClasse1 = (Elem*) malloc(sizeof(Elem));
	Elem *noClasse2 = (Elem*) malloc(sizeof(Elem));
	Elem *noClasse3 = (Elem*) malloc(sizeof(Elem));

	if (no->prox != NULL)
		*lista = no->prox;
	else
		exit(3);

	if(no == NULL)
		exit (1);

	while (no != NULL){
		if (no->dados->bitR == 0 && no->dados->bitM == 0){
			classe0 = true;
			break;
		} else {
			if (classe1 == false && no->dados->bitR == 0 && no->dados->bitM == 1){
					classe1 = true;
					noClasse1 = no;
			} else {
				if (classe1 == false && classe2 == false &&
					no->dados->bitR == 1 && no->dados->bitM == 0){
						classe2 = true;
						noClasse2 = no;
					} else {
						if (classe1 == false && classe2 == false && classe3 == false &&
							no->dados->bitR == 1 && no->dados->bitM == 1){
								classe3 = true;
								noClasse3 = no;
							}
					}
			}
		}

		/*Retira a pagina do inicio da lista e adiciona ao final*/
		dadosPag *dadosAux;
		dadosAux = (dadosPag*) malloc(sizeof(dadosPag));

		dadosAux->posicao = no->dados->posicao;
		dadosAux->offset = no->dados->offset;
		no = no->prox;
		erro = insere_lista_final(lista, dadosAux);
		if (erro == 0)
			exit(1);
		erro = remove_lista_inicio(lista);
		if (erro == 0)
			exit(1);
	}

		/* Procura na tabela o correspondente*/
		if(classe0 == false && classe1 == true){
			no = noClasse1;
		}	else {
			if (classe0 == false && classe1 == false && classe2 == true){
				no = noClasse2;
			} else {
				if (classe0 == false && classe1 == false && classe2 == false){
					no = noClasse3;
				}
			}
		}
		if (no == NULL)
			exit(5);
		Item *pag = memoriaVirtual->itens[no->dados->posicao];
		Item *anterior = pag;
		while (pag->prox != NULL && removeu == false){
			if (pag->prox->valor->offset == no->dados->offset){
				/*Remove da tabela*/
				if (pag->prox->valor->dirty == 1)
					writebacks++;
				pag = pag->prox;
				anterior->prox  = pag->prox;
				removeu = true;
				//no = no->prox;
				*lista = no->prox;
				//erro = remove_lista_inicio(lista);
				pag_usadas --;
			}
			if (removeu == false){
				anterior = pag;
				pag = pag->prox;
			}
		}
	if (removeu == true){
		if (erro == 0)
			exit(1);
		/*Adiciona a página*/
		Pagina *nova  = (Pagina*) calloc(1, sizeof(Pagina));
		Item *pag = memoriaVirtual->itens[posicao];
		dadosPag *dados;
		dados = (dadosPag*) malloc(sizeof(dadosPag));

		while (pag->prox != NULL)
			pag = pag->prox;

		nova->offset = endereco_tmp;
		dados->offset = endereco_tmp;
		dados->posicao = posicao;
		dados->bitR = 1;
		if (escrita == true)
			dados->bitM = 1;

		pag->prox = novoItem(&endereco_tmp, nova);
		pag_usadas ++;
		erro = insere_lista_final(lista, dados);
		if (erro == 0)
			exit(1);
	}
}

void LRU (int endereco_tmp){
	bool removeu = false;
	Elem *no = (Elem*) malloc(sizeof(Elem));
	no = *lista;

	if (no->prox != NULL)
		*lista = no->prox;
	else
		exit(3);

	if(no == NULL)
		exit(2);

		/* Procura na tabela o correspondente*/
		Item *pag = memoriaVirtual->itens[no->dados->posicao];
		Item *anterior = pag;
		while (pag->prox != NULL && removeu == false){
			if (pag->prox->valor->offset == no->dados->offset){
				/*Remove da tabela*/
				if (pag->prox->valor->dirty == 1)
					writebacks++;
				pag = pag->prox;
				anterior->prox  = pag->prox;
				removeu = true;
				*lista = no->prox;
				pag_usadas --;
			}
			if (removeu == false){
				anterior = pag;
				pag = pag->prox;
			}
		}
	if (removeu == true){
		if (erro == 0)
			exit(1);
		/*Adiciona a página*/
		Pagina *nova  = (Pagina*) calloc(1, sizeof(Pagina));
		Item *pag = memoriaVirtual->itens[posicao];
		dadosPag *dados;
		dados = (dadosPag*) malloc(sizeof(dadosPag));

		while (pag->prox != NULL)
			pag = pag->prox;

		nova->offset = endereco_tmp;
		dados->offset = endereco_tmp;
		dados->posicao = posicao;

		pag->prox = novoItem(&endereco_tmp, nova);
		pag_usadas ++;
		erro = insere_lista_final(lista, dados);
		if (erro == 0)
			exit(1);
	}
}

void substitui_pagina(int endereco_tmp){
	if(strcmp(algoritmo, "nru") == 0){
		NRU(endereco_tmp);
	}
	if(strcmp(algoritmo, "lru") == 0){
		LRU(endereco_tmp);
	}
	if(strcmp(algoritmo, "segunda_chance") == 0){
		segunda_chance(endereco_tmp);
	}
}

int main(int argc, char *argv[]){

	clock_t Ticks[2];
  Ticks[0] = clock();

	algoritmo = argv[1];
	arquivo = argv[2];
	tam_pag = atoi(argv[3]);
  tam_mem = atoi(argv[4]);

  // Verifica se os argumentos são validos
	if (argc != 5){
    perror("É aceito apenas quatro argumento de linha de comando.");
    exit(1);
  }
	if(tam_pag < 2 || tam_pag > 64){
		perror("O tamanho de cada pagina deve estar entre 2 e 64.\n");
		exit(1);
	}
	if(tam_mem < 128 || tam_mem > 16384){
		perror("O tamanho da memoria deve estar entre 128 e 16384.\n");
		exit(1);
	}
	if(strcmp(algoritmo, "lru") && strcmp(algoritmo, "nru") && strcmp(algoritmo,"segunda_chance")){
		perror("O algoritmo deve ser lru, nru ou segunda chance.\n");
		exit(1);
	}
	// Numero de paginas a ser criada
	num_pag = tam_mem/tam_pag;
	memoriaVirtual = novaTabela(&num_pag);
	lista = cria_lista();

	//encontrando o deslocamento
	unsigned s, tmp;
	tmp = tam_pag * 1024;
	s = 0;
	while (tmp > 1){
		tmp = tmp>>1;
		s++;
	}
	int cont = 0;
	if(strlen(arquivo) > 0){
		file = fopen(arquivo, "r");
		int addr;
		char rw;
		if(!file){
			printf("%s\n", arquivo);
			perror("Erro ao abrir o arquivo.\n");
			exit(1);
		}
		while(!feof(file) && fscanf(file,"%x %c", &addr, &rw)){
			operacoes++;
			endereco_tmp = addr >> s;
			posicao = endereco_tmp % memoriaVirtual->tamanho;
			cont ++;
			if(rw == 'W' || rw == 'w'){
				//printf("%d\n", cont);
				escrita = true;
				escritas ++;
				procuraPagina(endereco_tmp);
			} else {
				if(rw == 'R' || rw == 'r'){
					escrita = false;
					leituras ++;
					//printf("%d\n", cont);
					procuraPagina(endereco_tmp);
				} else {
					perror("Os dados do arquivo de entrada estao em formato incorreto.");
					exit(1);
				}
			}
		}
	}
	else {
		perror("ERRO: Arquivo de entrada inválido.");
		exit(1);
	}
  printf(">> Executando o simulador: \n");
  printf("%-35s %s\n", "Nome do arquivo:", arquivo);
	printf("%-35s %dKB\n", "Tamanho da memoria:", tam_mem);
	printf("%-35s %dKB\n", "Tamanho das paginas:", tam_pag);
	printf("%-35s %s\n", "Tecnica de reposicao:", algoritmo);
	printf("%-35s %d\n", "Numero de paginas:", num_pag);
	printf("%-35s %d\n", "Operacoes no arquivo de entrada:", operacoes-1);
	printf("%-35s %d\n", "Operacoes de leitura:", leituras);
	printf("%-35s %d\n", "Operacoes de escrita:", escritas);
	printf("%-35s %d\n", "Page hits:", acertoPag);
	printf("%-35s %d\n", "Page misses:", faltaPag);
	printf("%-35s %d\n", "Numero de writebacks:", writebacks);
	printf("%-35s %f%% \n", "Taxa de page fault:", faults/escritas*100.0);

  Ticks[1] = clock();
  double Tempo = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;
  printf("%-35s %g ms\n","Tempo gasto:", Tempo);

	return 0;
}
