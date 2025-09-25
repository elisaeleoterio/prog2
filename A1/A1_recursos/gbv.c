#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util.h"
#include "gbv.h"


/* ANOTAÇÕES:
A estrutura Document armazena os metadados dos arquivos
A estrutura Library armazena a lista de todos os Documentos
Stream é uma estrutura de dados que conecta os dispositivos (transefrência sequencial de dados) de entrada e saída
Criar funções para lidar com erros diferentes que se repetem
Verificar parâmetros da função fopen

Escrever os metadados no lib->docs
Achar onde a Área de Dados termina -> Ponteiro no início do diretório indica onde está
fread para pegar todos os metadados e colocá-los no local que o ponteiro de Document aponta para


PERGUNTAS:
Eu preciso criar uma estrutura específica para armazenar o cabeçalho?
Não entendi se meu cálculo do offset está correto.
Essa forma de implementação do ponteiro para alib->docs, que só apresenta o ponteiro para o início do vetor é realmente assim?
O que é o archive e o que é o document da função gbv_add?
Onde estão os metadados do arquivo?
Não entendi exatamente a função remover. Eu removo logicamente o arquivo, mas não os dados. Quando eu for visualizar os arquivos, eu não imprimo esse que foi removido daí?
Ao substituir um arquivo na função add, precisa ser adicionado no mesmo lugar?
Não entendi a função que está definida em util.c
Como limitar o tamanho do nome do arquivo ao adicionar nos metadados?
Como abrir a biblioteca no remover?
Falta archive no gbv_view também?

*/

// FUNÇÕES AUXILIARES
int buscar(const Library *lib, const char *docname) {
    // Passar pelos metadados
    // Comparar strings
    // Caso seja igual, retornar
    for (size_t i = 0; i < lib->count; i++) {
        if (strcmp(docname, lib->docs[i].name) == 0) {
            return i;
        }
    }
    return -1; // Posição inválida do vetor
}


// FUNÇÕES GBV

// Cria um diretório no armazenamento interno (disco) e o popula com as informações de cabeçalho
// Retorna 0 em caso de sucesso e 1 em caso de erro
int gbv_create(const char *filename) {
    if (!filename) {
        printf("Ponteiro nulo.\n");
        return 1;
    }

    // Cria o arquivo com permissão de escrita
    FILE *f = fopen(filename, "w+b");
    if (!f) {
        printf("Erro ao criar diretório.\n");
        return 1;
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
// Retorna 0 em caso de operação bem sucedida e 1 em caso de erro
int gbv_open(Library *lib, const char *filename) {
    
    FILE *f = fopen(filename, "rb"); // rb é leitura de arquivo binário
    
    // Se não existe, cria
    if (f == NULL) { 
        if (gbv_create(filename)) {
            printf("Erro ao criar diretório.\n");
            return 1;
        }
        // Abre o arquivo criado para leitura
        f = fopen(filename, "rb");
        if (!f) {
            printf("Erro ao criar diretório.\n");
        }
        
    }
    
    // Armazena na struct a quantidade de arquivos que estão no diretório
    int count;
    fseek(f, 0, SEEK_SET);
    fread(&count, sizeof(int), 1, f);
    lib->count = count;

    if (lib->count) {
        // Cria um ponteiro para um vetor que armazena os metadados dos arquivos
        lib->docs = malloc(lib->count * sizeof(Document));
        if (!lib->docs) {
            printf("Não foi possível alocar espaço na memória.\n");
            fclose(f);
            return 1;
        } 
    
        // Escrever os metadados no vetor lib->docs
        long offset;
        fseek(f, sizeof(int), SEEK_SET); // Coloca o início do stream após o fim da primeira parte do cabeçalho
        fread(&offset, sizeof(long), 1, f); // Offset para fim da Área de Dados
        fseek(f, offset, SEEK_SET); // Busca onde a área de dados finaliza
        fread(lib->docs, sizeof(Document), lib->count, f); // Lê os metadados e os armazena em lib->docs        
    } else {
        lib->docs = NULL;
    }
    

    return fclose(f);
}

// Adiciona um Documento à biblioteca no fima da Área de Dados
// Retorna 0 no caso de operação bem sucedida
int gbv_add(Library *lib, const char *archive, const char *docname) {
    if (!lib || !archive || !docname) {
        printf("Ponteiro nulo.\n");
        return 1;
    }

    // Abre o arquivo novo para leitura
    FILE *novo = fopen(docname, "rb"); 
    if (!novo) {
        printf("Arquivo inexistente.\n");
        return 1;
    }
    
    // Verifica se arquivo já existe na biblioteca, 
    // Se sim, remove ele
    if (buscar(lib, docname) > -1) {
        if(gbv_remove(lib, archive, docname)) {
            printf("Erro ao remover arquivo repetido.\n");
            fclose(novo);
            return 1;
        }
    }

    // Achar tamanho do arquivo novo
    fseek(novo, 0, SEEK_END);
    long tam_novo = ftell(novo); // Tamanho do arquivo em bytes
    fseek(novo, 0, SEEK_SET);
    
    // Criar buffer com tamanho do novo arquivo
    void *buffer = malloc(tam_novo);

    // Copiar arquivo novo pro buffer
    fread(buffer, tam_novo, 1, novo);


    // Abre o diretório onde irá adicionar o arquivo com permissão de escrita
    FILE *f = fopen(archive, "r+b"); 
    if (!f) {
        printf("Biblioteca inexistente.\n");
        fclose(novo);
        free(buffer);
        return 1;
    }

    // Achar offset onde devo adicionar o arquivo
    fseek(f, sizeof(int), SEEK_SET); // Coloca o início do stream após o fim da primeira parte do cabeçalho
    long offset;
    fread(&offset, sizeof(long int), 1, f); // Vai ler offset do diretório
    fseek(f, offset, SEEK_SET); // Busca onde a área de dados finaliza

    // Escrever arquivo novo na memória
    fwrite(buffer, tam_novo, 1, f);
    
    free(buffer);

    // Fecha arquivo novo
    fclose(novo);

    // Definir os metadados
    Document metadados;
    strcpy(metadados.name, docname); // Até 256B
    metadados.date = time(NULL); // Data de inserção
    metadados.offset = offset; // Offset do container (onde o arquivo anterior termina e o novo começa)
    metadados.size = tam_novo; // Tamanho em bytes

    // Atualizar quantidade de arquivos
    lib->count++;

    // Aumentar espaço para os metadados
    lib->docs = realloc(lib->docs, lib->count * sizeof(Document));
    if (!lib->docs) {
        printf("Erro ao alocar.\n");
        return 1;
    }
    
    // Escrever novo metadado no último espaço do vetor
    lib->docs[lib->count - 1] = metadados;

    // Acha onde os metadados devem ser escritos (offset + tamanho do novo arquivo)
    fseek(f, offset + tam_novo, SEEK_SET);
    // Escreve metadados no fim do arquivo
    fwrite(lib->docs, sizeof(Document), lib->count, f);
    
    // Atualiza cabeçalho
    fseek(f, 0, SEEK_SET); // Coloca para o início do arquivo
    fwrite(&lib->count, sizeof(int), 1, f); // Escreve a quantidade de arquivos
    fseek(f, sizeof(int), SEEK_SET); // Coloca para após o dado da quantidade de aquivos
    long novo_offset = metadados.offset + tam_novo;
    fwrite(&novo_offset, sizeof(long), 1, f); // Escreve o offset para o fim da área de dados
    

    return fclose(f);
}

// Remove logicamente os documentos indicados (os dados
// permanecem no arquivo, mas o metadado é excluído).
int gbv_remove(Library *lib, const char *archive, const char *docname) {
    if (!lib || !archive || !docname) {
        printf("Ponteiro nulo.\n");
        return -1;
    }

    // Busca posição do arquivo na biblioteca (busca nos metadaddos)
    int pos = buscar(lib, docname);
    if (pos == -1) {
        printf("Arquivo inexistente na biblioteca.\n");
        return 0;
    }

    void *buffer = malloc(2 * BUFFER_SIZE);
    if (!buffer) {
        printf("Erro ao alocar.\n");
        return -1;
    }
    
    // Abre a biblioteca com permissão para leitura e escrita
    FILE *f = fopen(archive, "r+b");
    // Passa por todos os arquivos depois do arquivo a ser removido
    for (size_t i = pos + 1; i < lib->count; i++) {
        fseek(f, lib->docs[i].offset, SEEK_SET); // Encontra onde o arquivo posterior começa
        fread(buffer, lib->docs[i].size, 1, f); // Escreve arquivo no buffer
        
        fseek(f, lib->docs[i - 1].offset, SEEK_SET); // Aponta para onde a escrita deve ser feita
        long novo_offset = ftell(f);
        fwrite(buffer, lib->docs[i].size, 1, f); // Substitui o arquivo anterior
        lib->docs[i].offset = novo_offset; // Atualiza o metadado do arquivo movido
    }

    // Substituir metadados no vetor da Library
    for (size_t i = pos + 1; i < lib->count; i++) {
        lib->docs[i - 1] = lib->docs[i];
    }
    
    // Atualização do tamanho do vetor
    lib->docs = realloc(lib->docs, lib->count - 1);

    // Escrever metadados na biblioteca
    long fim_dados = lib->docs[lib->count - 1].offset + lib->docs[lib->count - 1].size; // Definir offset do fim da Área de Dados
    fseek(f, fim_dados, SEEK_SET); // Alterar para apontar para lá
    fwrite(lib->docs, sizeof(Document), lib->count, f); // Escrever metadados

    fseek(f, 0, SEEK_SET); // Apontar para início da biblioteca

    // Atualiza cabeçalho
    fseek(f, 0, SEEK_SET); // Coloca para o início do arquivo
    fwrite(&lib->count, sizeof(int), 1, f); // Escreve a quantidade de arquivos
    fseek(f, sizeof(int), SEEK_SET); // Coloca para após o dado da quantidade de aquivos
    fwrite(&fim_dados, sizeof(long), 1, f); // Escreve o offset para o fim da área de dados

    return fclose(f);
}

// Lista os documentos, exibindo: nome; tamanho em bytes; data
// de inserção; posição (offset) no container.
int gbv_list(const Library *lib) {
    if (!lib) {
        printf("Ponteiro nulo.\n");
        return -1;
    }
    
    for (size_t i = 0; i < lib->count; i++) {
        printf("------ ARQUIVO %ld ------\n", i);
        printf("Nome: %s\n", lib->docs[i].name);
        printf("Tamanho em bytes: %ld\n", lib->docs[i].size);
        printf("Data de Inserção: ");
        // format_date(lib->docs[i].date, buffer?, max?);
        printf("Posição: %ld\n", lib->docs[i].offset);
    }
    
    return 0;
}

// Visualiza o documento em blocos de tamanho fixo. O
// usuário deve poder navegar pelo conteúdo:
//     ◦ n → próximo bloco,
//     ◦ p → bloco anterior,
//     ◦ q → sair da visualização.
// int gbv_view(const Library *lib, const char *archive, const char *docname) {
//     if (!lib || !docname) {
//         printf("Ponteiro nulo.\n");
//         return -1;
//     }

//     FILE *f = fopen(archive, "rb");
//     if (!f) {
//         printf("Arquivo inexistente.\n");
//         return -1;
//     }

//     // Buscar posição do arquivo a ser lido
//     int pos = buscar(lib, docname);

//     // Seta para o início do arquivo a ser lido
//     fseek(f, lib->docs[pos].offset, SEEK_SET);

//     // Aloca vetor para ler arquivo
//     void *buffer = malloc(2 * BUFFER_SIZE);
//     if (!buffer) {
//         printf("Erro ao alocar.\n");
//         return -1;
//     }

//     // Lê arquivo para o buffer
//     fread(buffer, lib->docs[pos].size, 1, f);


    


    


//     // Ler próxima navegação
//     char nav;
//     do
//     {
//         printf("Selecione a próxima navegação: \n");
//         printf("◦ n → próximo bloco\n ◦ p → bloco anterior\n ◦ q → sair da visualização\n");
//         scanf("%c", &nav);
//     } while (nav != "n" || nav != "p" || nav != "q");
    
    
    


    
//}