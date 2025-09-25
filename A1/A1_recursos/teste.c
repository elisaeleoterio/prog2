#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura de metadados de cada documento
typedef struct {
    char name[256];   // nome do documento
    long size;             // tamanho em bytes
    time_t date;           // data de inserção
    long offset;           // posição no container
} Document;

// Estrutura que representa a biblioteca (diretório em memória)
typedef struct {
    Document *docs;        // vetor dinâmico de documentos
    int count;             // número de documentos
} Library;


// Cria um diretório no armazenamento interno (disco) e o popula com as informações de cabeçalho
// Retorna 0 em caso de sucesso e 1 em caso de erro
int gbv_create(const char *filename) {
    if (!filename) {
        printf("Ponteiro nulo.\n");
        return -1;
    }

    // Cria o arquivo com permissão de escrita
    FILE *f = fopen(filename, "w+b");
    if (!f) {
        printf("Erro ao criar diretório.\n");
        return -1;
    }
    
    int tam = 0; // Definição da quantidade de arquivos no diretório
    // Escrita inicial do cabeçalho (quantidade de arquivos)
    fseek(f, 0, SEEK_SET);
    fwrite(&tam, sizeof(int), 1, f);


    // Definição do offset de onde inicia a Área de Dados da biblioteca
    long int dir_offset = sizeof(int) + sizeof(long int); 
    fseek(f, sizeof(int), SEEK_SET);
    // Escrita do offset no cabeçalho do diretório
    fwrite(&dir_offset, sizeof(long), 1, f);
    
    return fclose(f);
}

// Carrega o diretório da biblioteca para a memória.
// Caso não exista, cria o diretório
// Retorna 0 em caso de operação bem sucedida
int gbv_open(Library *lib, const char *filename) {
    
    FILE *f = fopen(filename, "rb"); // rb é leitura de arquivo binário
    
    // Se não existe, cria
    if (f == NULL) { 
        if (!gbv_create(filename)) {
            printf("Erro ao criar diretório.\n");
            return -1;
        }
        
        // Abre o arquivo criado para leitura
        f = fopen(filename, "rb");
    }
    
    // Armazena na struct a qauntidade de arquivos que estão no diretório
    int count;
    fread(&count, sizeof(int), 1, f);
    lib->count = count;

    // Cria um ponteiro para um vetor que armazena os metadados dos arquivos
    lib->docs = malloc(lib->count * sizeof(Document));
    if (!lib->docs) {
        printf("Não foi possível alocar espaço na memória.\n");
        return -1;
    } 

        // Escrever os metadados no vetor
        /*
        Achar onde a Área de Dados termina -> Ponteiro no início do diretório indica onde está
        fread para pegar todos os metadados e colocá-los no local que o ponteiro de Document aponta para
        */

    long offset;
    fseek(f, sizeof(int), SEEK_SET); // Coloca o início do stream após o fim da primeira parte do cabeçalho
    fread(&offset, sizeof(long int), 1, f); // Vai escrever offset do diretório
    fseek(f, offset, SEEK_SET); // Busca onde a área de dados finaliza
    fread(lib->docs, sizeof(Document), lib->count, f); // Lê os metadados e os armazena em lib->docs

    return fclose(f);
}

int main() {
    // printf("Testando gbv_create...\n");
    
    // // Teste 1: Criar arquivo normal
    // int result = gbv_create("test.gbv");
    // printf("Resultado: %d\n", result);
    
    // // Teste 2: Verificar se arquivo foi criado
    // FILE *f = fopen("test.gbv", "rb");
    // if (f) {
    //     printf("✓ Arquivo criado com sucesso\n");
        
    //     // Ler e verificar conteúdo
    //     int count;
    //     long offset;
        
    //     fread(&count, sizeof(int), 1, f);
    //     fread(&offset, sizeof(long), 1, f);
        
    //     printf("Count: %d (esperado: 0)\n", count);
    //     printf("Offset: %ld (esperado: %ld)\n", offset, sizeof(int) + sizeof(long));
        
    //     fclose(f);
    // } else {
    //     printf("✗ Erro: arquivo não foi criado\n");
    // }
    
    // // Teste 3: Ponteiro nulo
    // result = gbv_create(NULL);
    // printf("Teste ponteiro nulo: %d (esperado: -1)\n", result);
    
    return 0;
}