#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pelucia.h"

// Funções auxiliares
int aleat(int min, int max) {
    int valor = (rand() % (max - min + 1)) + min;
    return valor;
}

// Funções extras
struct maquina_pelucia *cria_maquina(int id) {

    struct maquina_pelucia *maq = malloc(sizeof(struct maquina_pelucia));
    if(maq == NULL) {
        printf("Erro ao alocar máquina.\n");
        return NULL;
    }

    maq->id = id;
    maq->probabilidade = aleat(0, 100);
    maq->proximo = NULL;
    maq->anterior = NULL;

    return maq;
}


void retira_maq(struct loja *loja, struct maquina_pelucia *maq) {
    if (loja == NULL || maq == NULL || loja->numero_maquinas <= 0) {
        return;
    }

/*    struct maquina_pelucia *atual = loja->inicio;
    do {
        if(atual->id == maq->id) {
            break;
        }  
        atual = atual->proximo;
    } while (atual != loja->inicio);


    if(atual->id != maq->id) {
        return;
    }
*/
    // maquina com só uma lista
    if (maq->proximo == maq) {
        loja->inicio = NULL;
    } else {
        maq->anterior->proximo = maq->proximo;
        maq->proximo->anterior = maq->anterior;
        
        // caso for a primeira máquina
        if (maq == loja->inicio) {
            loja->inicio = maq->proximo;
        }
    }
    
    loja->numero_maquinas--;
    free(maq);
}

void inserir_maquina(struct loja *loja, struct maquina_pelucia *maq) {
    if (loja == NULL || maq == NULL) {
        printf("Ponteiro nulo\n");
        return;
    }

    
    // inserir em fila vazia
    if(loja->inicio == NULL) {
        loja->inicio = maq;
        maq->anterior = maq;
        maq->proximo = maq;

        loja->numero_maquinas++;
        return;
    }
    
    // inserir no início da fila
    if (maq->probabilidade >= loja->inicio->probabilidade) {
        maq->anterior = loja->inicio->anterior;
        maq->proximo = loja->inicio;
        loja->inicio->anterior->proximo = maq;
        loja->inicio->anterior = maq;
        loja->inicio = maq;
        loja->numero_maquinas++;
        return;
    }

    // inserir no meio da fila
    struct maquina_pelucia *aux = loja->inicio;
    
    while ( aux->proximo != loja->inicio && maq->probabilidade < aux->proximo->probabilidade) {
        aux = aux->proximo;
    }

    maq->anterior = aux;
    maq->proximo = aux->proximo;
    aux->proximo->anterior = maq;
    aux->proximo = maq;
    
    loja->numero_maquinas++;
}

// Funções essenciais (.h)
struct loja* criar_loja (unsigned int numero_maquinas) {
    struct loja *loj = malloc(sizeof(struct loja));
    if(loj == NULL) {
        printf("Erro ao alocar ponteiro de loja.\n");
        return NULL;
    }

    srand(0);

    loj->inicio = NULL;
    loj->numero_maquinas = 0;
    
    for (size_t i = 0; i < numero_maquinas; i++) {
        struct maquina_pelucia *maq = cria_maquina(i);
        inserir_maquina(loj, maq);
    }

    return loj;   
}

int jogar (struct loja *loja) {
    if(loja == NULL || loja->numero_maquinas == 0) {
        return -1;
    }

    // sortear máquina
    unsigned int num_maq = aleat(0, loja->numero_maquinas-1);

    // achar a maquina
    struct maquina_pelucia *maq = loja->inicio;
    for (size_t i = 0; i < num_maq; i++) {
        maq = maq->proximo;
    }
    
    if(maq == NULL) {
        printf("Erro ao buscar máquina\n");
        return -1;
    }
    
    // sortear numero do jogador
    unsigned int num_jogador = aleat(0, 100);
    
    if(num_jogador < maq->probabilidade) {
        retira_maq(loja, maq);
        return 1;
    }
    return 0;
}


void encerrar_dia (struct loja *loja) {
    if(loja == NULL) {
        printf("Ponteiro nulo.\n");
        return;
    }
    if (loja->numero_maquinas <= 0) {
        printf("Loja Vazia.\n");
        printf("FIM!\n");
        return;
    }
    
    struct maquina_pelucia *aux = loja->inicio;
    for (size_t i = 0; i < loja->numero_maquinas; i++) {
        printf("Máquina número [%ld]: \n", i);
        printf("ID da máquina: %d\n", aux->id);
        printf("Probabilidade da máquina: %d\n", aux->probabilidade);
        printf("\n");
        aux = aux->proximo;
    }
}

void destruir_loja (struct loja *loja) {
    if(loja == NULL) {
        return;
    }

    while(loja->numero_maquinas > 0 && loja->inicio != NULL) {
        retira_maq(loja, loja->inicio);
    }
    free(loja);
}