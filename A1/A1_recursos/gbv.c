#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util.h"
#include "gbv.h"

#define BUFFER_SIZE [tamanho do meu maior arquivo]


/* ANOTAÇÕES:
A estrutura Document armazena os metadados dos arquivos
A estrutura Library armazena a lista de todos os Documentos
Stream é uma estrutura de dados que conecta os dispositivos (transefrência sequencial de dados) de entrada e saída
Criar funções para lidar com erros diferentes que se repetem

PERGUNTAS:
Eu preciso criar uma estrutura específica para armazenar o cabeçalho?
Não entendi se meu cálculo do offset está correto.
Essa forma de implementação do ponteiro para alib->docs, que só apresenta o ponteiro para o início do vetor é realmente assim?
O que é o archive e o que é o document da função gbv_add?
Onde estão os metadados do arquivo?
Não entendi exatamente a função remover. Eu removo logicamente o arquivo, mas não os dados. Quando eu for visualizar os arquivos, eu não imprimo esse que foi removido daí?
*/

// FUNÇÕES GBV

// Cria um diretório no armazenamento interno (disco) e o popula com as informações de cabeçalho
// Retorna 0 em caso de sucesso
int gbv_create(const char *filename) {
    
    if (!filename) {
        printf("Ponteiro nulo.\n");
        return -1;
    }

    // Cria o arquivo com permissão de escrita
    FILE *f = fopen(filename, "wb"); // wb é abrir o arquivo com permissão de escrita binária
    if (!f) {
        printf("Erro ao criar diretório.\n");
        return -1;
    }
    
    int tam = 0; // Definição da quantidade de arquivos no diretório
    // Escrita inicial do cabeçalho (quantidade de arquivos)
    if(fwrite(&tam, sizeof(int), 1, f)) {
        printf("Erro ao escrever dados.\n");
        return -1;
    }; 

    long int dir_offset = sizeof(int) + sizeof(long int); // Definição do offset de onde inicia as informações dos arquivos
    // Escrita do offset no cabeçalho do diretório
    if(fwrite(&dir_offset, sizeof(long), 1, f)) {
        printf("Erro ao escrever dados.\n");
        return -1;
    };
    
    fclose(f);
    return 0;
}

// Carrega o diretório da biblioteca para a memória.
// Caso não exista, cria o diretório
// Retorna 0 em caso de operação bem sucedida
int gbv_open(Library *lib, const char *filename) {
    
    FILE *f = fopen(filename, "rb"); // rb é leitura de arquivo binário
    
    // Se não existe, cria
    if (f == NULL) { 
        if (gbv_create(filename)) {
            printf("Erro ao criar diretório.\n");
            return -1;
        }
        
        // Abre o arquivo criado para leitura
        f = fopen(filename, "rb");
    }
    
    // Armazena na struct a qauntidade de arquivos que estão no diretório
    int count;
    fread(count, sizeof(int), 1, f);
    lib->count = count;
    if (lib->count) { // Se existirem arquivos no diretório
        
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
        fread(lib->docs, sizeof(lib->count * sizeof(Document)), 1, f); // Lê os metadados e os armazena em lib->docs
    }

    fclose(f);
    return 0;
}

// Adiciona um Documento à biblioteca no fima da Área de Dados
// Retorna 0 no caso de operação bem sucedida
int gbv_add(Library *lib, const char *archive, const char *docname) {

    /*
    Abrir diretório (archive)
    Abrir arquivo novo (docname)
    Verificar se já existe um arquivo com esse nome no diretório
        Se sim, substituir
            Verificar tamanho do novo documento e comparar com o tamanho do anterior
                se maior
                    copiar o de baixo para o buffer
                    escrever o novo
                    copiar o de baixo do de baixo
                    copiar o de baixo
                    ...
                    atualizar metadados
                se menor
                    escrever novo no lugar
                    jogar todos os demais arquivos para cima
                    atualizar metadados

        se não
            Passar novo arquivo para o buffer (docname)
            Escrever arquivo do buffer para o diretório na memória de disco
            Adicionar metadados na lib->docs
            Escrever lib->docs na parte de metadados do diretório
            Atualizar quantidade de arquivos do lib->count
            Atualizar quantidade de arquivos do cabeçalho do diretório
    fclose
    */

    /* METADADOS
    char name[MAX_NAME];   // nome do documento
    long size;             // tamanho em bytes
    time_t date;           // data de inserção
    long offset;           // posição no container
    */

    FILE *novo = fopen(docname, "rb");
    if (!novo) {
        fprintf(stderr, "Erro ao abrir arquivo.\n");
        return -1;
    }
    
}

int gbv_remove(Library *lib, const char *docname) {
    /*
    r : remove logicamente os documentos indicados (os dados
    permanecem no arquivo, mas o metadado é excluído).
    Remove apenas os metadados??????

    */
}

int gbv_list(const Library *lib) {
    /*
    lista os documentos, exibindo: nome; tamanho em bytes; data
    de inserção; posição (offset) no container.
    */
}

int gbv_view(const Library *lib, const char *docname) {
    /*
    visualiza o documento em blocos de tamanho fixo. O
    usuário deve poder navegar pelo conteúdo:
        ◦ n → próximo bloco,
        ◦ p → bloco anterior,
        ◦ q → sair da visualização.
    */
}