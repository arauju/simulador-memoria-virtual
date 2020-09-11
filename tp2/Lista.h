#ifndef LISTA_H
#define LISTA_H

#include <stdlib.h>
#include <stdio.h>

typedef struct DadosPag {
  int posicao;
  unsigned offset;
  int bitR;
  int bitM;
}dadosPag;

struct elemento{
    int cont;
    int tamanho;
    struct elemento *ant;
    struct elemento *prox;
    dadosPag *dados;
};

typedef struct elemento Elem;

typedef struct elemento* Lista;

Lista* cria_lista();
void libera_lista(Lista* li);
int insere_lista_final(Lista* li, dadosPag *pag);
int insere_lista_inicio(Lista* li, dadosPag *pag);
int remove_lista_inicio(Lista* li);
int remove_lista_final(Lista* li);
void imprime_lista(Lista* li);
/*int consulta_lista_pos(Lista* li, int pos, struct aluno *al);
int consulta_lista_mat(Lista* li, int mat, struct aluno *al);
int insere_lista_ordenada(Lista* li, struct aluno al);
int remove_lista(Lista* li, int mat);

int tamanho_lista(Lista* li);
int lista_vazia(Lista* li);
void imprime_lista(Lista* li);*/

#endif
