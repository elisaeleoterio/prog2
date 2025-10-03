#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util.h"
#include "gbv.h"

#define BLOCK_SIZE 500 // Definição para o tamanho da visualização na função view

// FUNÇÕES AUXILIARES

// Busca nos metadados o arquivo de mesmo nome
// Retorna a posição do arquivo na biblioteca ou -1 em caso de não existir
int buscar(const Library *lib, const char *docname) {
    if (!lib || !docname) {
        printf("Ponteiros nulos.\n");
        return -1;
    }
    
    for (int i = 0; i < lib->count; i++) {
        if (strcmp(docname, lib->docs[i].name) == 0) {
            return i;
        }
    }

    // Posição inválida do vetor
    return -1; 
}


// IMPLEMENTAÇÃO DAS FUNÇÕES GBV

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
    
    // Escrita inicial do cabeçalho

    int tam = 0;
    fseek(f, 0, SEEK_SET);
    fwrite(&tam, sizeof(int), 1, f);

    long int dir_offset = sizeof(int) + sizeof(long int); 

    fseek(f, sizeof(int), SEEK_SET);
    fwrite(&dir_offset, sizeof(long), 1, f);
    
    return fclose(f);
}

// Abre a STREAM FILE* do diretório.
// Caso não exista, cria o diretório
// Retorna 0 em caso de operação bem sucedida e 1 em caso de erro
int gbv_open(Library *lib, const char *filename) {
    if (!lib || !filename) {
        printf("Ponteiro nulo.\n");
        return 1;
    }
    
    // Abre a stream do arquivo para leitura
    FILE *f = fopen(filename, "rb"); 
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

    // Caso tenha arquivos no diretório
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
        fseek(f, sizeof(int), SEEK_SET); 
        fread(&offset, sizeof(long), 1, f);
        fseek(f, offset, SEEK_SET);
        fread(lib->docs, sizeof(Document), lib->count, f);
    // Caso o diretório esteja vazio
    } else {
        lib->docs = NULL;
    }

    return fclose(f);
}

// Adiciona um Documento à biblioteca no fima da Área de Dados
// Retorna 0 no caso de operação bem sucedida e 1 em caso de erro
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
    // Se sim, remove ele para adicionar posteriormente
    if (buscar(lib, docname) > -1) {
        if(gbv_remove(lib, archive, docname)) {
            printf("Erro ao remover arquivo repetido.\n");
            fclose(novo);
            return 1;
        }
    }

    // Abre o diretório onde irá adicionar o arquivo com permissão de escrita e leitura
    FILE *f = fopen(archive, "r+b"); 
    if (!f) {
        printf("Biblioteca inexistente.\n");
        fclose(novo);
        return 1;
    }

    // Definir tamanho do arquivo novo
    fseek(novo, 0, SEEK_END);
    size_t tam_novo = ftell(novo);
    fseek(novo, 0, SEEK_SET);
    

    // Encontra o local do fim da Área de Dados
    long offset;
    fseek(f, sizeof(int), SEEK_SET);
    fread(&offset, sizeof(long), 1, f);
    fseek(f, 0, SEEK_SET);

    // Caso não for um arquivo vazio
    if (tam_novo > 0) { 
        // Cria buffer com tamanho do arquivo
        void *buffer = malloc(tam_novo);
        if (!buffer) {
            printf("Erro ao alocar.\n");
            fclose(novo);
            fclose(f);
            return 1;
        }
        
        // Copiar arquivo novo pro buffer
        size_t lidos = fread(buffer, 1, tam_novo, novo);
        if (lidos != tam_novo) {
            printf("Erro ao ler arquivo de origem.\n");
            free(buffer);
            fclose(novo);
            fclose(f);
            return 1;
        }             

        // Achar offset de onde o arquivo deve ser adicionado
        fseek(f, sizeof(int), SEEK_SET);
        fread(&offset, sizeof(long), 1, f); 
        fseek(f, offset, SEEK_SET);


        // Escrever arquivo novo na memória
        size_t escritos = fwrite(buffer, 1, lidos, f);
        if (escritos != lidos) {
            printf("Erro ao escrever arquivo na biblioteca.\n");
            free(buffer);
            fclose(f);
            return 1;
        }
        
        free(buffer);
    }
    
    // Fecha arquivo novo
    fclose(novo);

    // Define os metadados
    Document metadados;
    memset(&metadados, 0, sizeof(Document)); // "Limpa" o dado para não salva lixo de memória
    strcpy(metadados.name, docname); // Até 256B
    metadados.date = time(NULL); // Data de inserção
    metadados.offset = offset; // Offset do container (onde o arquivo anterior termina e o novo começa)
    metadados.size = tam_novo; // Tamanho em bytes

    // Atualiza quantidade de arquivos
    lib->count++;

    // Aumentar espaço para os metadados no vetor
    Document *temp = realloc(lib->docs, lib->count * sizeof(Document));
    if (!temp) {
        printf("Erro ao alocar.\n");
        fclose(f);
        return 1;
    }
    lib->docs = temp;
    
    // Escreve novo metadado no último espaço do vetor
    lib->docs[lib->count - 1] = metadados;

    // Define onde os metadados devem ser escritos no diretório
    long meta = offset + tam_novo;
    fseek(f, meta, SEEK_SET);
    // Escreve metadados no fim do arquivo
    fwrite(lib->docs, sizeof(Document), lib->count, f);
    
    // Atualiza cabeçalho
    fseek(f, 0, SEEK_SET);
    fwrite(&lib->count, sizeof(int), 1, f);
    fseek(f, sizeof(int), SEEK_SET); 
    
    long novo_offset = metadados.offset + tam_novo;
    fwrite(&novo_offset, sizeof(long), 1, f); 
    

    return fclose(f);
}

// Remove os documentos indicados.
// Retorna 0 em caso de sucesso e 1 em caso de erro
int gbv_remove(Library *lib, const char *archive, const char *docname) {
    if (!lib || !archive || !docname) {
        printf("Ponteiro nulo.\n");
        return 1;
    }

    // Busca posição do arquivo na biblioteca
    int pos = buscar(lib, docname);
    if (pos == -1) {
        printf("Arquivo inexistente na biblioteca.\n");
        return 0;
    }

    // Abre a biblioteca com permissão para leitura e escrita
    FILE *f = fopen(archive, "r+b");

    // Caso for o único arquivo da biblioteca
    if (lib->count <= 1) {

        lib->count--;

        // Escreve cabeçalho
        fseek(f, 0, SEEK_SET);
        fwrite(&lib->count, sizeof(int), 1, f);

        fseek(f, sizeof(int), SEEK_SET);
        long novo_offset = sizeof(int) + sizeof(long);
        fwrite(&novo_offset, sizeof(long), 1, f);

        free(lib->docs);
    } else {
        
        void *buffer; 
    
        // Move os arquivos posteriores para cima
        for (int i = pos + 1; i < lib->count; i++) {
            
            buffer = malloc(lib->docs[i].size);  
            if (!buffer) {
                printf("Erro ao alocar.\n");
                return 1;
            }        

            // Encontra onde o arquivo posterior começa e o escreve no buffer
            fseek(f, lib->docs[i].offset, SEEK_SET); 
            fread(buffer, lib->docs[i].size, 1, f); 
            
            // Busca onde deve ser escrito e realiza a substituição
            fseek(f, lib->docs[i - 1].offset, SEEK_SET); 
            fwrite(buffer, lib->docs[i].size, 1, f); 

            free(buffer);
        }
        
        // Substituir metadados no vetor
        for (int i = pos + 1; i < lib->count; i++) {
            lib->docs[i - 1] = lib->docs[i];
        }

        long novo_offset = sizeof(int) + sizeof(long); 
        for (int i = 0; i < lib->count; i++) {
            lib->docs[i].offset = novo_offset;
             novo_offset += lib->docs[i].size;
        }
        
        lib->count--;

        // Atualiza tamanho do vetor
        Document *temp = realloc(lib->docs, lib->count * sizeof(Document));
        if (!temp) {
            printf("Erro ao realocar docs.\n");
            fclose(f);
            return 1;
        }
        lib->docs = temp;

        // Escrever metadados na biblioteca
        long fim_dados = lib->docs[lib->count - 1].offset + lib->docs[lib->count - 1].size; 
        fseek(f, fim_dados, SEEK_SET); 
        fwrite(lib->docs, sizeof(Document), lib->count, f); 
    
        // Atualiza cabeçalho
        fseek(f, 0, SEEK_SET);
        fwrite(&lib->count, sizeof(int), 1, f);
        fseek(f, sizeof(int), SEEK_SET); 
        fwrite(&fim_dados, sizeof(long), 1, f); 
    }

    return fclose(f);
}

// Lista os documentos, exibindo: nome; tamanho em bytes; data
// de inserção; posição (offset) no container.
// Retorna 0 em caso de sucesso e 1 em caso de erro
int gbv_list(const Library *lib) {
    if (!lib) {
        printf("Ponteiro nulo.\n");
        return 1;
    }

    if (lib->count <= 0) {
        printf("Não há arquivos na biblioteca ainda.\n");
    } else {
        for (int i = 0; i < lib->count; i++) {
            printf("\n");
            printf("------ ARQUIVO %d ------\n", i);
            printf("Nome: %s\n", lib->docs[i].name);
            printf("Tamanho em bytes: %ld\n", lib->docs[i].size);
            char data[50];
            format_date(lib->docs[i].date, data, sizeof(data));
            printf("Data de Inserção: %s\n", data);
            printf("Posição: %ld\n", lib->docs[i].offset);
            printf("\n");
        }        
    }

    return 0;
}

// Visualiza o documento em blocos de tamanho fixo. O
// usuário deve poder navegar pelo conteúdo:
//     ◦ n → próximo bloco,
//     ◦ p → bloco anterior,
//     ◦ q → sair da visualização.
// Retorna 0 em caso de sucesso e 1 em caso de erro
int gbv_view(const Library *lib, const char *archive, const char *docname) {
    if (!lib || !archive || !docname) {
        printf("Ponteiro nulo.\n");
        return 1;
    }

    // Abre arquivo para leitura
    FILE *f = fopen(archive, "rb");
    if (!f) {
        printf("Arquivo inexistente.\n");
        return 1;
    }
    
    // Encontra o incio do arquivo (será usado como condição para não ler lixo de memória)    
    fseek(f, 0, SEEK_SET);
    long inicio = ftell(f);

    // Encontra o fim do arquivo
    fseek(f, 0, SEEK_END);
    long fim = ftell(f);

    // Buscar posição do arquivo a ser lido
    int pos = buscar(lib, docname);
    if (pos == -1) {
        printf("Documento não existe na biblioteca.\n");
        fclose(f);
        return 1;
    }
    
    // Aloca vetor para ler arquivo
    char *buffer = malloc(BLOCK_SIZE);
    if (!buffer) {
        printf("Erro ao alocar.\n");
        return 1;
    }

    // Define o início do bloco a ser lido
    fseek(f, lib->docs[pos].offset, SEEK_SET);
    long offset = ftell(f);
    printf("Offset inicial: %ld\n", offset);

    //Escreve pedaço de tamanho BLOCK_SIZE do arquivo no buffer
    fread(buffer, BLOCK_SIZE, 1, f);

    // Imprime trecho do arquivo para o usuário
    printf("----------------------------------------\n");
    printf("%s\n", buffer);
    printf("----------------------------------------\n");

    // Início da navegação do view
    char nav;
    printf("Navegação → n (próximo bloco) | p (bloco anterior) | q (sair da visualização)\n");
    printf("Escolha a próxima operação: ");
    scanf(" %c", &nav);
    while (nav != 'q') {
        // Seguir para próximo bloco
        if (nav == 'n') {
             // Pega a posição atual do ponteiro do file e soma para ir para o próximo bloco
            offset = offset + BLOCK_SIZE;
            if (offset > fim) {
                printf("Você chegou ao fim da biblioteca.\n");
                fclose(f);
                free(buffer);
                return 1;
            }

            //Escreve pedaço de tamanho BLOCK_SIZE do arquivo no buffer
            fseek(f, offset, SEEK_SET);
            fread(buffer, BLOCK_SIZE, 1, f);

            // Imprime trecho do arquivo para o usuário
            printf("----------------------------------------\n");
            printf("%s\n", buffer);
            printf("----------------------------------------\n");
        // Retornar ao bloco anterior
        } else if (nav == 'p') {
            // Pega a posição atual do ponteiro do file e subtraí para ir para o próximo bloco
            offset = offset - BLOCK_SIZE;
            if (offset < inicio) {
                printf("Você chegou antes do início da biblioteca.\n");
                fclose(f);
                free(buffer);
                return 1;
            }

            //Escreve pedaço de tamanho BLOCK_SIZE do arquivo no buffer
            fseek(f, offset, SEEK_SET);
            fread(buffer, BLOCK_SIZE, 1, f);

            // Imprime trecho do arquivo para o usuário
            printf("----------------------------------------\n");
            printf("%s\n", buffer);
            printf("----------------------------------------\n");
        }
        printf("Escolha a próxima operação: ");
        scanf(" %c", &nav);
        printf("\n");
    }

    free(buffer);
    return 0;
}