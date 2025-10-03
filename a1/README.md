# README - Trabalho A1 Gerenciador de Biblioteca Virtual (gbv)

> **Autora**: Elisa Rocha Eleotério 
> **GRR**: 20241731

---

## 📁 Lista de arquivos:

- `gbv.c` ................ Implementações das funções centrais do Gerenciador de Biblioteca Virtual.
- `gbv.h` ................ Cabeçalho com as definições das estruturas de dados e das funções utilizadas para gerenciar a biblioteca virtual.
- `main.c` ............ Arquivo principal de execução do projeto. Responsável por interpretar os argumentos da linha de comando e chamar as funções apropriadas de gbv.c.
- `util.h` ............ Protótipo para função de formatação de data.  
- `util.c` ............ Implementação de função para formatação de data.  
- `Makefile` ............. Arquivo de compilação automática do trabalho através do comando make.  
- `README` ............... Este documento.
- `teste-uso.sh` ......... Script de testes de usabilidade e execução das funções do trabalho.
- `teste-valgrind.sh`......... Script de testes iguais aos do teste-uso com verificação de vazamento de memório e erros do programa.  

---

## ⚙️ Lógica de funcionamento:

O programa gbv realiza a aglutinação de arquivos dentro de um único arquivo (.gbv). A estrutura desse diretório é dividida em três partes principais:

    1. Cabeçalho:
    Localizado no início do arquivo, contém a quantidade de documentos no diretório e o offset para onde os arquivos terminam e a parte dos metadados inicia.

    2. Área de Dados:
    Onde o conteúdo binário bruto de cada documento é armazenado sequencialmente. Novos arquivos são sempre adicionados no final desta área.

    3. Área de Diretório (metadados): 
    Localizada no final do arquivo .gbv. Armazena um vetor de metadados (struct Document) com as informações de cada arquivo: nome, tamanho, data de inserção e o seu offset de início em relação ao início do Cabeçalho.

Ao iniciar, o programa lê o cabeçalho, aloca memória para o diretório e carrega estes metadados para a RAM. As operações de adição e remoção manipulam o arquivo em disco e o diretório em memória de forma síncrona.

---

## 🧱 Dificuldades encontradas:

Durante o desenvolvimento, surgiram várias dúvidas, tanto conceituais quanto de implementação. 

### Entendimento da Estrutura do Arquivo .gbv
- A maior dificuldade inicial foi compreender como o cabeçalho, os dados e o diretório de metadados seriam estruturados no diretório .gbv e como estes se referenciavam dentro de um único arquivo binário.
    > - Solução: Desenhar a estrutura geral do arquivo e como cada elemento se relacionava.

### Gerenciamento e cálculo do offset
- Determinar o offset e como era alterado em cada função foi complicado, especialmente para compreender o uso das funções fseek e ftell.

### Uso do vetor de armazenamento dos metadados
- Por ser um ponteiro que aponta apenas para o início do vetor que armazena os metadados, o entendimento sobre como o uso do ponteiro deveria ser feito foi complicado.

---

## 🐞 Bugs conhecidos:

- A função de adicionar um arquivo repetido não realiza a substituição do arquivo, apenas o remove e então o adiciona no fim do diretório. 

---

## Casos de Teste para o Programa GBV

Dois scripts foram elaborados para validar todas as funcionalidades do programa gbv, incluindo o tratamento de erros, casos de borda e o comportamento esperado de cada operação, conforme a especificação do projeto.

- `teste-uso.sh` Realiza testes de usabilidade e resultados das operações (adicionar, reover, listar e visualizar).
- `teste-valgrind.sh` Realiza testes para verificar vazamentos de memória e erros de implementação no código.

---

## Fontes Utilizadas de Base

Além dos materiais das aulas, utilizei sites e recursos disponíveis na internet para entender melhor como utilizar as ferramentas para o trabalho.

### Entender funções da biblioteca stdio:
- https://www.tutorialspoint.com/c_standard_library/
- https://petbcc.ufscar.br/stdio/
- https://www.ibm.com/docs/pt-br/i/7.5.0?topic=files-stdioh
