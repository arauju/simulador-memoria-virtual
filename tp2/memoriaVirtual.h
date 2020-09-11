#ifndef MEMORIAVIRTUAL_H
#define MEMORIAVIRTUAL_H


//void LRU (int endereco_tmp, int posicao);
//void addPagina(int endereco_tmp, int posicao);
//void NRU (int endereco_tmp, int posicao);
int segunda_chance (int endereco_tmp);
void substitui_pagina(int endereco_tmp);
void LRU (int endereco_tmp);
void NRU (int endereco_tmp);

#endif
