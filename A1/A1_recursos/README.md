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

O arquivador VINAc foi implementado com as seguintes estruturas:

- `struct archive`: representa um arquivo armazenado no arquivador, contendo:
  - Nome do arquivo.
  - ID do arquivo.
  - Tamanho do arquivo.
  - Tamanho antes da compressão.
  - Última vez que ele foi modificado.
  - Deslocamento (offset) onde o arquivo está armazenado no `.vc`.
  - Se ele está comprimido ou não.

- `struct directory`: mantém o diretório com:
  - O número de arquivos.
  - Um vetor dinâmico de `archive` que armazena os arquivos.

O diretório é sempre armazenado **no início** do arquivo `.vc`. Para cada inserção, o arquivo original é lido e um novo arquivo é criado com o diretório atualizado e todos os dados dos arquivos. Os offsets dos arquivos são recalculados conforme a ordem de inserção, e a estrutura do diretório também é modificada.

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