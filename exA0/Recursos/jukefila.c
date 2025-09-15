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
    if(!contar_jukefila(fila)) {
        printf("Fila Vazia\n");
        fila->inicio = elemento;
        fila->final = elemento;
        elemento->anterior = NULL;
        elemento->proximo = NULL;

        return;
    }

    // adicionar no inicio da fila
    if(elemento->valor > fila->inicio->valor) {
        printf("Inicio da Fila\n");
        elemento->proximo = fila->inicio;
        fila->inicio = elemento;
        elemento->anterior = NULL;
        
        return;
    }

    //adicionar no final da fila
    if (elemento->valor < fila->final->valor) {
        printf("Fim da Fila\n");
        elemento->anterior = fila->final;
        elemento->proximo = NULL;
        fila->final = elemento;

        return;
    }
    


    pedido *pedido_atual = fila->inicio;
    // inserir entre a fila
    while (pedido_atual) {
        if(elemento->valor >= pedido_atual->valor) {
            printf("No meio\n");
            elemento->proximo = pedido_atual;
            elemento->anterior = pedido_atual->anterior;
            pedido_atual->anterior = elemento;
            
            return;
        }
        pedido_atual = pedido_atual->proximo;
    }

    printf("ERRO ao inserir\n");
}

pedido* consumir_jukefila(jukefila* fila) {
    pedido *proximo = fila->inicio;

    pedido *aux = fila->inicio->proximo;
    fila->inicio = aux;
    aux->anterior = NULL;

    return proximo;
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
    if (contar_jukefila(fila) <= 0) {
    printf("Fila vazia.\n");
    return;
    }

    pedido *pedido_atual = fila->inicio;
    for(unsigned int i = 0; i < contar_jukefila(fila); i++) {
        pedido *aux = pedido_atual->proximo;
        destruir_pedido(pedido_atual);
        free(pedido_atual);
        pedido_atual = aux;
    }

    free(fila);

}
