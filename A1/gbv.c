#include <stdio.h>
#include <stdlib.h>

#include "gbv.h"

int gbv_create(const char *filename) {

    
}


int gbv_open(Library *lib, const char *filename) {
    // Pass por todos os arquivos da biblioteca e os abre para leitura
    for (size_t i = 0; i < lib->count; i++) {
        // Abra um arquivo de texto para leitura. O arquivo deve existir.
        if (fopen("filename/lib->docs->name", "r") == NULL) {
            printf("Arquivo inexistente.\n");
        }
    }
    
}

// Insere um ou mais documentos na biblioteca. Se já existir um
// documento com o mesmo nome, ele deve ser substituído.
int gbv_add(Library *lib, const char *archive, const char *docname) {
    //Crie um arquivo de texto para leitura e gravação. 
    //Se o arquivo fornecido existir, seu conteúdo será limpo, a menos que seja um arquivo lógico.
    if (fopen(docname, "w+")) {
        for (size_t i = 0; i < lib->count; i++)
        {
            if (strlib->docs[i]->name)
            {
                /* code */
            }
            
        }
        
    }
    
}
int gbv_remove(Library *lib, const char *docname);
int gbv_list(const Library *lib);
int gbv_view(const Library *lib, const char *docname);


// int gbv_order(Library *lib, const char *archive, const char *criteria);


// int fclose(FILE* stream)
// Se for == 0, então operação bem sucedida