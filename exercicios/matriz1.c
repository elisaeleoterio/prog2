// Utilizando alocação dinâmica de matrizes, crie um programa para receber duas matrizes de tamanho 3×3 e calcular a multiplicação delas.

#include <stdio.h>
#include <stdlib.h>

#define COL 3
#define LIN 3

int **criar_matriz(int lin, int col) {
     // Alocação da matriz de forma dinâmica
    int **mat;

    mat = calloc(lin, sizeof(int*));

    for(size_t i = 0; i < lin; i++) {
        mat[i] = malloc(col * sizeof(int));
    }

    return mat;
}

void ler_matriz(int **mat, int lin, int col) {
    for (size_t i = 0; i < lin; i++) {
        for (size_t j = 0; j < col; j++) {
            printf("Elemento [%d][%d]: ", i, j);
            scanf("%d", &mat[i][j]);
            printf("\n");
        }
        
    }
}

void imprimir_matriz(int **mat, int lin, int col) {
    if (!mat) {
        return;
    }
    printf("%d", mat[1][1]);
    for (size_t i = 0; i < lin; i++) {
        for (size_t j = 0; j < col; j++) {
            //printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

int **multi_matriz(int **matA, int **matB, int linA, int colA, int linB, int colB) {
    if (colA != linB) {
        printf("Condição de existência de multiplicação não satisfeito.\n");
        return NULL;
    }

    int **matC = criar_matriz(linA, colB);

    // passar pelos espaços da matric C
    for (size_t i = 0; i < linA; i++) {
        for (size_t j = 0; j < colB; j++) {
            // Multiplicar linha da primeira pela coluna da segunda e somar valores
            for (size_t k = 0; k < linA; k++) {
                    matC[i][j] = matC[i][j] + matA[i][k] * matB[k][j];
            }
        }
    }

    return matC;
}

int **libera_matriz(int **matriz, int lin, int col) {
    for(size_t i = 0; i < lin; i++) {
        free(matriz[i]);
    }

    free(matriz);
}

int main() {
    
    int **mat1 = criar_matriz(LIN, COL);
    printf("VALORES DA MATRIZ 1: \n");
    ler_matriz(mat1, LIN, COL);

    int **mat2 = criar_matriz(LIN, COL);
    printf("VALORES DA MATRIZ 2: \n");
    ler_matriz(mat2, LIN, COL);

    printf("MATRIZ 1: \n");
    imprimir_matriz(mat1, LIN, COL);

    printf("MATRIZ 2: \n");
    imprimir_matriz(mat2, LIN, COL);
    
    int **mat3 = multi_matriz(mat1, mat2, LIN, COL, LIN, COL);

    imprimir_matriz(mat3, COL, LIN);
    
    mat1 = libera_matriz(mat1, LIN, COL);
    mat2 = libera_matriz(mat2, LIN, COL);
    mat3 = libera_matriz(mat3, LIN, COL);

}