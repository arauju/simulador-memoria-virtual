#include "tabelaHash.h"

Item* novoItem (unsigned *chave, Pagina *valor){
    Item* item = malloc (sizeof(Item));
    item->chave = chave;
    item->valor = valor;
    return item;
}

TabelaHash* novaTabela (int *tamanho){
    TabelaHash* tabela = malloc(sizeof(TabelaHash));

    tabela->tamanho = *tamanho;
    tabela->itens = calloc(tabela->tamanho, sizeof(Item*));

    for(int i = 0; i < *tamanho ; i++)
    	tabela->itens[i] = (Item*) calloc(1, sizeof(Item));

    return tabela;
}

void deletaItem (Item *item){
    free(item->valor);
    free(item->chave);
    free(item);
}

void deletaTabela (TabelaHash *tabela){
    for (int i = 0; i < tabela->tamanho; i++){
        Item *item = tabela->itens[i];
        if (item != NULL)
            deletaItem(item);
    }

    free(tabela->itens);
    free(tabela);
}

void imprimeTabela(TabelaHash *memoriaVirtual){
  printf("\nTABELA:\n");
  for (int i = 0; i < memoriaVirtual->tamanho; i++){
    while (memoriaVirtual->itens[i]->prox != NULL){
      printf("%d \t", memoriaVirtual->itens[i]->prox->valor->offset);
      memoriaVirtual->itens[i] = memoriaVirtual->itens[i]->prox;
    }
  }
}
