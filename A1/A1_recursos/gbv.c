#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util.h"
#include "gbv.h"

#define BLOCK_SIZE 500

/* ANOTAÇÕES:
A estrutura Document armazena os metadados dos arquivos
A estrutura Library armazena a lista de todos os Documentos
Stream é uma estrutura de dados que conecta os dispositivos (transefrência sequencial de dados) de entrada e saída
Criar funções para lidar com erros diferentes que se repetem
Verificar parâmetros da função fopen

Escrever os metadados no lib->docs
Achar onde a Área de Dados termina -> Ponteiro no início do diretório indica onde está
fread para pegar todos os metadados e colocá-los no local que o ponteiro de Document aponta para

ERROS ATUAIS
- Adição de arquivo repetido está adicionando duas vezes
- Remoção não está correta inteiramente
- View não está funcionando corretamente


PERGUNTAS:
// Eu preciso criar uma estrutura específica para armazenar o cabeçalho?
// Não entendi se meu cálculo do offset está correto.
// O que é o archive e o que é o document da função gbv_add?
// Onde estão os metadados do arquivo?
// Não entendi a função que está definida em util.c
// Essa forma de implementação do ponteiro para a lib->docs, que só apresenta o ponteiro para o início do vetor é realmente assim?

Não entendi exatamente a função remover. Eu removo logicamente o arquivo, mas não os dados. Quando eu for visualizar os arquivos, eu não imprimo esse que foi removido daí?
Ao substituir um arquivo na função add, precisa ser adicionado no mesmo lugar?
    Precisa
Como limitar o tamanho do nome do arquivo ao adicionar nos metadados?
Como abrir a biblioteca no remover?
Falta archive no gbv_view também?
Por que eu não consigo transformar o main.c em um arquivo gbv?
Não está dando free no lib->docs


Não é permitido carregar um documento inteiro na memória?
Posso alterar o main?
Na gbv_remove, se o arquivo não estiver no diretório, eu retorno como erro?
*/

// FUNÇÕES AUXILIARES

// Busca nos metadados o arquivo de mesmo nome
// Retorna a posição do arquivo na biblioteca ou -1 em caso de não existir
int buscar(const Library *lib, const char *docname) {
    if (!lib || !docname) {
        
    }
    
    for (int i = 0; i < lib->count; i++) {
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
    
    // Escrita inicial do cabeçalho

    int tam = 0;
    fseek(f, 0, SEEK_SET);
    fwrite(&tam, sizeof(int), 1, f);

    // Definição do offset de onde inicia a Área de Dados da biblioteca
    long int dir_offset = sizeof(int) + sizeof(long int); 

    fseek(f, sizeof(int), SEEK_SET);
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
    
    // DÚVIDA NESSA IMPLEMENTAÇÃO DE SUBSTITUIÇÃO
    // Verifica se arquivo já existe na biblioteca, 
    // Se sim, remove ele
    if (buscar(lib, docname) > -1) {
        printf("Arquivo repetido.\n");
        if(gbv_remove(lib, archive, docname)) {
            printf("Erro ao remover arquivo repetido.\n");
            fclose(novo);
            return 1;
        }
    }

    // Abre o diretório onde irá adicionar o arquivo com permissão de escrita
    FILE *f = fopen(archive, "r+b"); 
    if (!f) {
        printf("Biblioteca inexistente.\n");
        fclose(novo);
        return 1;
    }

    // Definir tamanho do arquivo novo
    fseek(novo, 0, SEEK_END);
    long tam_novo = ftell(novo); // Tamanho do arquivo em bytes
    fseek(novo, 0, SEEK_SET);
    

    long offset;
    if (tam_novo > 0) {
        // Criar buffer com tamanho do novo arquivo
        void *buffer = malloc(2 * BUFFER_SIZE);
        if (!buffer) {
            printf("Erro ao alocar.\n");
            fclose(novo);
            fclose(f);
            return 1;
        }
        
        // Copiar arquivo novo pro buffer
        fread(buffer, tam_novo, 1, novo);        

        // Achar offset onde devo adicionar o arquivo
    
        // Coloca o início do stream após o fim da primeira parte do cabeçalho
        fseek(f, sizeof(int), SEEK_SET);
        fread(&offset, sizeof(long int), 1, f); // Vai ler offset do diretório
        fseek(f, offset, SEEK_SET); // Busca onde a área de dados finaliza

        // Escrever arquivo novo na memória
        fwrite(buffer, tam_novo, 1, f);
        
        free(buffer);
    }
    
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
    Document *temp = realloc(lib->docs, lib->count * sizeof(Document));
    if (!temp) {
        printf("Erro ao alocar.\n");
        fclose(f);
        return 1;
    }
    lib->docs = temp;
    
    // Escrever novo metadado no último espaço do vetor
    lib->docs[lib->count - 1] = metadados;

    // Acha onde os metadados devem ser escritos no diretório (offset + tamanho do novo arquivo)
    fseek(f, offset + tam_novo, SEEK_SET);
    // Escreve metadados no fim do arquivo
    fwrite(lib->docs, sizeof(Document), lib->count, f);
    
    // Atualiza cabeçalho
    fseek(f, 0, SEEK_SET); // Coloca para o início do arquivo
    fwrite(&lib->count, sizeof(int), 1, f);
    fseek(f, sizeof(int), SEEK_SET); 
    
    long novo_offset = metadados.offset + tam_novo;
    printf("Novo offset: %ld\n", novo_offset);
    fwrite(&novo_offset, sizeof(long), 1, f); // Escreve o offset para o fim da área de dados
    

    return fclose(f);
}

// DÚVIDA NA IMPLEMENTAÇÃO
// Remove logicamente os documentos indicados (os dados
// permanecem no arquivo, mas o metadado é excluído).
int gbv_remove(Library *lib, const char *archive, const char *docname) {
    if (!lib || !archive || !docname) {
        printf("Ponteiro nulo.\n");
        return 1;
    }

    // Busca posição do arquivo na biblioteca (busca nos metadaddos)
    int pos = buscar(lib, docname);
    if (pos == -1) {
        printf("Arquivo inexistente na biblioteca.\n");
        return 0;
    }

    // Abre a biblioteca com permissão para leitura e escrita
    FILE *f = fopen(archive, "r+b");

    // Caso for o único arquivo da biblioteca
    if (lib->count <= 1) {

        fseek(f, 0, SEEK_SET);
        lib->count--;
        fwrite(&lib->count, sizeof(int), 1, f);

        fseek(f, sizeof(int), SEEK_SET);
        long novo_offset = sizeof(int) + sizeof(long);
        fwrite(&novo_offset, sizeof(long), 1, f);

        free(lib->docs);
        
    } else {
        
        void *buffer; 
    
        // Move os arquivos posteriores para cima
        for (int i = pos + 1; i < lib->count; i++) {
            
            buffer = malloc(lib->docs[i].size); // DÚVIDA SOBRE ESSE TAMANHO   
            if (!buffer) {
                printf("Erro ao alocar.\n");
                return 1;
            }        

            // Encontra onde o arquivo posterior começa e o escreve no buffer
            fseek(f, lib->docs[i].offset, SEEK_SET); 
            fread(buffer, lib->docs[i].size, 1, f); 
            
            // Busca onde deve ser escrito, realiza a substituição e atualiza o metadado
            fseek(f, lib->docs[i - 1].offset, SEEK_SET); 
            long novo_offset = ftell(f);
            fwrite(buffer, lib->docs[i].size, 1, f); 
            lib->docs[i].offset = novo_offset; 


        }
        
        // Substituir metadados no vetor da Library
        for (int i = pos + 1; i < lib->count; i++) {
            lib->docs[i - 1] = lib->docs[i];
        }
        
        lib->count--;

        // Atualização do tamanho do vetor
        Document *temp = realloc(lib->docs, lib->count - 1);
        if (!temp) {
            printf("Erro ao realocar docs.\n");
            fclose(f);
            return 1;
        }
        lib->docs = temp;

        // Escrever metadados na biblioteca
        long fim_dados = lib->docs[lib->count - 1].offset + lib->docs[lib->count - 1].size; // Definir offset do fim da Área de Dados
        fseek(f, fim_dados, SEEK_SET); // Alterar para apontar para lá
        fwrite(lib->docs, sizeof(Document), lib->count, f); // Escrever metadados
    
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
int gbv_list(const Library *lib) {
    if (!lib) {
        printf("Ponteiro nulo.\n");
        return -1;
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
int gbv_view(const Library *lib, const char *archive, const char *docname) {
    if (!lib || !archive || !docname) {
        printf("Ponteiro nulo.\n");
        return -1;
    }

    // Abre arquivo para 
    FILE *f = fopen(archive, "rb");
    if (!f) {
        printf("Arquivo inexistente.\n");
        return -1;
    }
    
    // Encontra o incio do arquivo (será usado como condição para não ler lixo de memória)    
    fseek(f, 0, SEEK_SET);
    long inicio = ftell(f);

    // Encontra o fim do arquivo (será usado como condição para não ler lixo de memória)
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
    // Blocos serão do tamanho BLOCK_SIZE
    void *buffer = malloc(BLOCK_SIZE);
    if (!buffer) {
        printf("Erro ao alocar.\n");
        return -1;
    }

    // Seta para o início do bloco a ser lido
    fseek(f, lib->docs[pos].offset, SEEK_SET);

    //Escreve pedaço de tamanho BLOCK_SIZE do arquivo no buffer
    fread(buffer, BLOCK_SIZE, 1, f);

    // Imprime trecho do arquivo para o usuário
    printf("----------------------------------------\n");
    char *trecho = (char*)buffer; // Tranforma o ponteiro de void em uma string
    printf("%s\n", trecho);
    printf("----------------------------------------\n");

    char nav;
    scanf(" %c", &nav);
    while (nav != 'q') {
        if (nav == 'n') {
            pos = ftell(f) + BLOCK_SIZE; // Pega a posição atual do ponteiro do file e soma para ir para o próximo bloco
            if (pos > fim) {
                printf("Você chegou ao fim da biblioteca.\n");
                // Fecahr coisas tudo
                return 1;
            }

            fseek(f, pos, SEEK_SET);
            //Escreve pedaço de tamanho BLOCK_SIZE do arquivo no buffer
            fread(buffer, BLOCK_SIZE, 1, f);

            // Imprime trecho do arquivo para o usuário
            printf("----------------------------------------\n");
            trecho = (char*)buffer; // Tranforma o ponteiro de void em uma string
            printf("%s\n", trecho);
            printf("----------------------------------------\n");
            
        } else if (nav == 'p') {
            pos = ftell(f) - BLOCK_SIZE; // Pega a posição atual do ponteiro do file e soma para ir para o próximo bloco
            if (pos < inicio) {
                printf("Você chegou a antes do fim da biblioteca.\n");
                // Fecahr coisas tudo
                return 1;
            }

            fseek(f, pos, SEEK_SET);
            //Escreve pedaço de tamanho BLOCK_SIZE do arquivo no buffer
            fread(buffer, BLOCK_SIZE, 1, f);

            // Imprime trecho do arquivo para o usuário
            printf("----------------------------------------\n");
            trecho = (char*)buffer; // Tranforma o ponteiro de void em uma string
            printf("%s\n", trecho);
            printf("----------------------------------------\n");
        }
        scanf(" %c", &nav);
        printf("\n");
    }

    return 0;
}