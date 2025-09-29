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

- Função de adicionar arquivo repetido grande (ar1.txt) no diretório já criado está gerando segmentation fault. 

---

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

### Categoria 3: Testes da Operação de Remoção (-r)

1. Remover um arquivo do meio da biblioteca ( `./gbv -r bib_nova.gbv arq2.txt` )
2. Remover o primeiro arquivo da biblioteca ( `./gbv -r bib_nova.gbv arq1.txt` )
3. Remover o último arquivo da biblioteca ( `./gbv -r bib_nova.gbv arq3.txt` )
4. Remover um arquivo que não existe na biblioteca ( `./gbv -r bib_nova.gbv arquivo_fantasma.txt` )
5. Remover o único arquivo da biblioteca ( `./gbv -r bib_nova.gbv arquivo_unico.txt` )
6. Remover múltiplos arquivos de uma só vez ( `./gbv -r bib_nova.gbv arq1.txt arq3.txt` )

### Categoria 4: Testes da Operação de Listagem (-l)

1. Listar uma biblioteca vazia ( `./gbv -l bib_vazia.gbv` )
2. Listar uma biblioteca com vários arquivos ( `./gbv -l bib_cheia.gbv` )

### Categoria 5: Testes da Operação de Visualização (-v)

Testes interativos para verificar a execução da função de visualização.

1. Visualizar um arquivo maior que o BLOCK_SIZE ( `./gbv -v minhabib.gbv arquivo_grande.txt` )
2. Visualizar um arquivo menor que o BLOCK_SIZE ( `./gbv -v minhabib.gbv arquivo_pequeno.txt` )
3. Tentar navegar além dos limites ( `./gbv -v minhabib.gbv arquivo_grande.txt` )

            No primeiro bloco, pressionar p. O programa não deve quebrar; deve informar que está no início.

            No último bloco, pressionar n. O programa não deve quebrar; deve informar que está no fim.

 4. Visualizar um arquivo que não existe na biblioteca ( `./gbv -v minhabib.gbv arquivo_fantasma.txt` )
5. Visualizar um arquivo vazio ( `./gbv -v minhabib.gbv vazio.txt` )

## Testes com Valgrind

1. Criar e abrir um diretório não gera vazamento de memória algum
    - Comando utilizado: `valgrind --leak-check=full --track-origins=yes ./gbv -x bib.gbv ar1.txt`

2. Adicionar um arquivo de tamanho 100.339 bytes ( `ar1.txt` )
    - Comando utilizado: `valgrind --leak-check=full --track-origins=yes ./gbv -a bib.gbv ar1.txt`

3. Adicionar três arquivos
    - Comando utilizado: `valgrind --leak-check=full --track-origins=yes ./gbv -a bib.gbv ar1.tx ar2.txt ar3.txt`

## Fonte de Dúvidas
https://www.tutorialspoint.com/c_standard_library/