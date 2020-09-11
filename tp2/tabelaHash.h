#ifndef TABELAHASH_H
#define TABELAHASH_H

#include <stdio.h>
#include <stdlib.h>

typedef struct pagina{
  unsigned offset;
  int paginaAlterada;
  int dirty;
} Pagina;

typedef struct item {
  Pagina* valor;
  int *chave;
  struct item  *prox;
} Item;

typedef struct tabelaHash {
  Item **itens;
  int tamanho;
} TabelaHash;

Item* novoItem (unsigned *chave, Pagina *valor);
TabelaHash* novaTabela (int *tamanho);
void deletaItem (Item *item);
void deletaTabela (TabelaHash *tabela);
void imprimeTabela(TabelaHash *memoriaVirtual);

#endif
