#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct maquina_pelucia {
    unsigned int id;
    unsigned int probabilidade;

    struct maquina_pelucia *proximo;
    struct maquina_pelucia *anterior;
};

struct loja {
	struct maquina_pelucia *inicio;
	unsigned int numero_maquinas;
};

// Funções auxiliares
int aleat(int min, int max) {
    int valor = (rand() % (max - min + 1)) + min;
    return valor;
}

// Funções máquina de pelúcia
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
        return;
    }
    
    // inserir no início da fila
    if (maq->probabilidade > loja->inicio->probabilidade) {
        maq->anterior = loja->inicio->anterior;
        loja->inicio->anterior = maq;
        maq->proximo = loja->inicio;
        loja->inicio = maq;
        return;
    }

    // inserir no meio da fila
    struct maquina_pelucia *aux = loja->inicio;
    
    while (aux != NULL && maq->probabilidade < aux->probabilidade) {
        aux = aux->proximo;
    }
    maq->proximo = aux->proximo;
    maq->anterior = aux;
}

// Funções essenciais (.h)
struct loja* criar_loja (unsigned int numero_maquinas) {
    struct loja *loj = malloc(sizeof(struct loja));
    if(loj == NULL) {
        printf("Erro ao alocar ponteiro de loja.\n");
        return NULL;
    }

    srand(time(NULL));
    
    loj->inicio = NULL;
    loj->numero_maquinas = numero_maquinas;

    for (size_t i = 0; i < numero_maquinas; i++) {
        struct maquina_pelucia *maq = cria_maquina(i);
        inserir_maquina(loj, maq);
    }
    return loj;   
}
    

void imprime_loja(struct loja *loja) {
    if(loja == NULL) {
        printf("Ponteiro nulo.\n");
        return;
    }

    struct maquina_pelucia *aux = loja->inicio;
    for (size_t i = 0; i < loja->numero_maquinas; i++) {
        printf("Máquina número [%d]: \n", i);
        printf("ID da máquina: %d\n", aux->id);
        printf("Probabilidade da máquina: %d\n", aux->probabilidade);
        printf("\n");
        aux = aux->proximo;
    }
    
}

int main() {
    struct loja *loja = criar_loja(3);

    imprime_loja(loja);

    return 0;
}