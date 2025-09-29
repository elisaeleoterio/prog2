# README - Trabalho A1 Gerenciador de Biblioteca Virtual (gbv)

**Autora**: Elisa Rocha Eleotério 
**GRR**: 20241731

---

## 📁 Lista de arquivos:

- `gbv.c` ................ Implementações das funções do Gerenciador de Biblioteca Virtual;
- `gbv.h` ................ Cabeçalho com as definições das estruturas e das funções utilizadas para gerenciar a biblioteca virtual;
- `main.c` ............ Arquivo principal de execução do projeto.  
- `util.h` ............ Cabeçalho para função de formatação de data.  
- `util.c` ............ Implementação de função para formatação de data.  
- `Makefile` ............. Arquivo para compilar o projeto através do comando make.  
- `README` ............... Documento de identificação e relatório do desenvolvimento do projeto.  

---

## 🧠 Estruturas de dados e algoritmos utilizados:


---

## ⚙️ Lógica de funcionamento:

O arquivador VINAc foi implementado seguindo a seguinte lógica:

- Primeiro, é lido o buffer do terminal para saber qual operação deve ser feita.
- Em seguida, é usado um `switch case` para decidir a operação.
- No `insere`, é verificado se o `.vc` existe. Se não, cria ele e o diretório; se sim, lê o diretório atual. Após isso, ele itera nos argumentos do buffer do terminal, criando cada membro e inserindo. No final do loop, o diretório é escrito no começo.
- Na inserção com remoção, a lógica é a mesma. Porém, eu comprimo antes de inserir; caso a compressão falhe, o arquivo sem compressão é inserido.
- No `move`, primeiro procuramos o índice dos membros a serem movidos. Caso eles sejam inválidos, uma mensagem de erro é retornada. No caso de haver apenas um argumento, ele move para a primeira posição.
- Na `extração`, verificamos se existem argumentos no buffer do terminal. Se não, são extraídos todos os membros; se sim, eu verifico quais devem ser extraídos.
- O `remove` itera nos argumentos do buffer, procurando os membros a serem removidos.
- O `print` apenas verifica se o `.vc` existe. Se não, retorna uma mensagem de erro; se sim, imprime o conteúdo do diretório.

Também existem funções auxiliares para fazer as manipulações no vetor e no binário:

- A `move_data` movimenta o binário dos arquivos.
- A `update_index` arruma os índices do vetor em caso de `move` e `remove`.
- A `calc_offset` atualiza os offsets.
- A `buffer_size` ajusta o buffer padrão para o do maior membro. 
- O restante apenas cria e adiciona novos membros ao vetor.

---

### 💡 Alternativas consideradas:

- Usar apenas uma estrutura, mas achei mais organizado usar duas.
- Manter o diretório no final, mas isso foi descartado, pois considerei que a implementação seria mais complexa.

---

### 🧱 Dificuldades encontradas:

- Gerenciamento de offsets ao adicionar arquivos.
- Alocação dinâmica de arquivos e ajuste do vetor `arch` durante inserções.
- Uso das funções que manipulam arquivos.
- Dificuldade em como manipular o binário de forma correta.
- Dificuldades no uso da biblioteca lz.

---

## 🐞 Bugs conhecidos:

- Nos casos de teste do script.py não foram encontrados bugs. 

---

## Casos de Teste:

1. Argumentos insuficientes ou incorretos.
    - `./gbv`
    - `./gbv -a`
    - `./gbv -x bib.gbv` 
        Cria o diretório para armazenar, porém não realiza nenhuma das operações existentes
    - `./gbv -a bib.gbv`
        Cria o diretório para armazenar, porém não realiza nenhuma das operações existentes (não entra no loop que chama a função)

2. Adição de arquivos
    - Adição em um diretório vazio
        - `./gbv -a novo_dir.gbv arq1.txt`
            Cria o diretório 
    - Adição de arquivo vazio
        - `./gbv -a bib.gbv vazio.txt




## Casos de Teste para o Programa GBV

Esta lista foi elaborada para validar todas as funcionalidades do programa gbv, incluindo o tratamento de erros, casos de borda e o comportamento esperado de cada operação, conforme a especificação do projeto.

### Categoria 1: Testes de Usabilidade e Argumentos (Interface de Linha de Comando)

- Garantir que o programa lide corretamente com entradas inválidas do usuário.

1. Sem argumentos ( `./gbv` )
2. Argumentos insificientes ( `./gbv -a` )
3. Opção Inválida ( `./gbv -x minhabib.gbv arquivo.txt` )
4. Operação de visualização sem nome de documento ( `./gbv -v minhabib.gbv` )


### Categoria 2: Testes da Operação de Adição (-a)

- Garantir que a operação execute corretamente e sem erros

1. Adicionar um arquivo a uma biblioteca inexistente ( `./gbv -a bib_nova.gbv arq1.txt `)
2. Adicionar múltiplos arquivos de uma só vez ( `./gbv -a bib_nova.gbv arq2.txt arq3.txt`)
3. Substituir um arquivo existente ( `./gbv -a bib_nova.gbv arq1.txt` )

        Resultado Esperado: O arquivo arq1.txt na biblioteca deve ser substituído. Ao listar (-l), a data de inserção de arq1.txt deve ser mais recente que a anterior. O número total de arquivos na biblioteca não deve mudar.

4. Adicionar um arquivo que não existe no disco ( `./gbv -a bib_nova.gbv arquivo_fantasma.txt` )
5. Adicionar um arquivo vazio (0 bytes) ( `./gbv -a bib_nova.gbv vazio.txt` )

### Testes da Operação de Remoção (-r)

    TC-REM-01: Remover um arquivo do meio da biblioteca

        Contexto: Biblioteca com arq1.txt, arq2.txt, arq3.txt.

        Comando: ./gbv -r bib_nova.gbv arq2.txt

        Resultado Esperado: O arquivo arq2.txt deve ser removido. A listagem (-l) deve mostrar apenas arq1.txt e arq3.txt. Os offsets devem ser ajustados se a remoção for física (como no seu código).

    TC-REM-02: Remover o primeiro arquivo da biblioteca

        Comando: ./gbv -r bib_nova.gbv arq1.txt

        Resultado Esperado: arq1.txt é removido. A listagem mostra os arquivos restantes.

    TC-REM-03: Remover o último arquivo da biblioteca

        Comando: ./gbv -r bib_nova.gbv arq3.txt

        Resultado Esperado: arq3.txt é removido. A listagem mostra os arquivos restantes.

    TC-REM-04: Remover um arquivo que não existe na biblioteca

        Comando: ./gbv -r bib_nova.gbv arquivo_fantasma.txt

        Resultado Esperado: O programa deve exibir uma mensagem (ex: "Arquivo inexistente na biblioteca") e não fazer nenhuma alteração.

    TC-REM-05: Remover o único arquivo da biblioteca

        Contexto: Biblioteca com apenas um arquivo.

        Comando: Remover esse arquivo.

        Resultado Esperado: A biblioteca deve ficar vazia. A listagem (-l) não deve mostrar nenhum arquivo e o cabeçalho deve indicar 0 documentos.

    TC-REM-06: Remover múltiplos arquivos de uma só vez

        Comando: ./gbv -r bib_nova.gbv arq1.txt arq3.txt

        Resultado Esperado: Ambos os arquivos devem ser removidos.

Categoria 4: Testes da Operação de Listagem (-l)

    TC-LIST-01: Listar uma biblioteca vazia

        Comando: ./gbv -l bib_vazia.gbv

        Resultado Esperado: O programa não deve exibir nenhum arquivo, ou pode exibir uma mensagem como "A biblioteca está vazia". Não deve haver erros.

    TC-LIST-02: Listar uma biblioteca com vários arquivos

        Comando: ./gbv -l bib_cheia.gbv

        Resultado Esperado: Todos os arquivos devem ser listados, cada um com seu: nome, tamanho em bytes, data de inserção formatada e offset correto no container.

Categoria 5: Testes da Operação de Visualização (-v)

Estes testes são interativos.

    TC-VIEW-01: Visualizar um arquivo maior que o BLOCK_SIZE

        Comando: ./gbv -v minhabib.gbv arquivo_grande.txt

        Interação:

            Pressionar n: deve mostrar o próximo bloco de dados.

            Pressionar p: deve mostrar o bloco anterior.

            Pressionar q: deve sair do modo de visualização.

    TC-VIEW-02: Visualizar um arquivo menor que o BLOCK_SIZE

        Comando: ./gbv -v minhabib.gbv arquivo_pequeno.txt

        Interação: O conteúdo completo deve ser exibido. Pressionar n ou p não deve causar erro e, idealmente, deve informar que não há mais conteúdo naquela direção.

    TC-VIEW-03: Tentar navegar além dos limites

        Comando: ./gbv -v minhabib.gbv arquivo_grande.txt

        Interação:

            No primeiro bloco, pressionar p. O programa não deve quebrar; deve informar que está no início.

            No último bloco, pressionar n. O programa não deve quebrar; deve informar que está no fim.

    TC-VIEW-04: Visualizar um arquivo que não existe na biblioteca

        Comando: ./gbv -v minhabib.gbv arquivo_fantasma.txt

        Resultado Esperado: O programa deve exibir uma mensagem de erro e encerrar.

    TC-VIEW-05: Visualizar um arquivo vazio

        Comando: ./gbv -v minhabib.gbv vazio.txt

        Resultado Esperado: O programa deve mostrar um conteúdo vazio e sair com q sem erros.

Categoria 6: Testes de Robustez e Integridade

    TC-ROB-01: Operações com arquivos binários

        Contexto: Usar uma imagem JPG ou um arquivo .bin como documento.

        Comandos: Adicionar, listar, visualizar e remover o arquivo binário.

        Resultado Esperado: Todas as operações devem funcionar corretamente. A visualização (-v) mostrará "lixo" na tela (o que é esperado), mas não deve travar. Os cálculos de tamanho e offset devem estar corretos.

    TC-ROB-02: Sequência complexa de operações

        Contexto: Realizar uma série de adições e remoções de forma intercalada.

        Exemplo: Adicionar A, B, C -> Remover B -> Adicionar D -> Remover A -> Adicionar B (substituindo).

        Resultado Esperado: Ao final, a listagem (-l) deve refletir o estado correto da biblioteca, com os offsets e o contador de arquivos no cabeçalho devidamente atualizados a cada passo.

    TC-ROB-03 (Avançado): Teste com biblioteca corrompida

        Contexto: Abrir o arquivo .gbv com um editor hexadecimal e alterar manualmente o cabeçalho (ex: mudar a contagem de arquivos para um valor incorreto).

        Comando: Tentar executar qualquer operação (-l, -r, etc.).

        Resultado Esperado: O programa idealmente não deve ter uma "falha de segmentação". Ele pode exibir um erro de inconsistência ou simplesmente se comportar de maneira inesperada, mas o objetivo é verificar se ele não causa uma falha grave.