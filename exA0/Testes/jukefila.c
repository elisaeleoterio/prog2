#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jukefila.h"
#include "pedido.h"


jukefila* criar_jukefila() {
    jukefila *fila = malloc(sizeof(jukefila));
    if (fila == NULL) {
        printf("Erro ao alocar fila.\n");
        return NULL;
    }

    fila->inicio = NULL;
    fila->final = NULL;

    return fila;
}

void inserir_jukefila(pedido* elemento, jukefila* fila) {
    if(elemento == NULL || fila == NULL) {
        printf("Ponteiros nulos.\n");
        return;
    }
    
    // adicionar em fila vazia
    if(contar_jukefila(fila) == 0) {
        elemento->anterior = NULL;
        elemento->proximo = NULL;
        fila->inicio = elemento;
        fila->final = elemento;

        return;
    }

    // adicionar no inicio da fila
    if(elemento->valor > fila->inicio->valor) {
        elemento->proximo = fila->inicio;
        elemento->anterior = NULL;
        fila->inicio = elemento;
        
        return;
    }

    //adicionar no final da fila
    if (elemento->valor < fila->final->valor) {
        elemento->anterior = fila->final;
        elemento->proximo = NULL;
        fila->final = elemento;

        elemento->anterior->proximo = elemento;
        return;
    }
    


    pedido *pedido_atual = fila->inicio;
    // inserir entre a fila
    while (pedido_atual != NULL && pedido_atual->valor > elemento->valor) {
        pedido_atual = pedido_atual->proximo;
    }

    elemento->proximo = pedido_atual;
    elemento->anterior = pedido_atual->anterior;
    pedido_atual->anterior = elemento;
    elemento->anterior->proximo = elemento;
    
}

pedido* consumir_jukefila(jukefila* fila) {
    if(!contar_jukefila(fila)) {
        return NULL;
    }
    pedido *consumido = fila->inicio;

    fila->inicio = consumido->proximo;
    if(fila->inicio) {
        fila->inicio->anterior = NULL;
    }

    consumido->proximo = NULL;

    return consumido;
}

unsigned int contar_jukefila(jukefila* fila) {
    pedido *pedido_atual = fila->inicio;

    unsigned int count = 0;
    while (pedido_atual != NULL) {
        count++;
        pedido_atual = pedido_atual->proximo;
    }

    return count;
}

void destruir_jukefila(jukefila *fila) {

    pedido *pedido_atual = fila->inicio;
    pedido *aux = NULL;
    for(unsigned int i = 0; i < contar_jukefila(fila); i++) {
        aux = pedido_atual->proximo;
        destruir_pedido(pedido_atual);
        pedido_atual = aux;
    }

    free(fila);
}
