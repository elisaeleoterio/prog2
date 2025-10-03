#!/bin/bash

# Script para testar o programa gerenciador de biblioteca virtual (gbv).
#
# COMO USAR:
# 1. Certifique-se de que este script está no mesmo diretório que seus arquivos .c, .h e Makefile.
# 2. Dê permissão de execução ao script: chmod +x test_script.sh
# 3. Execute o script: ./test_script.sh

# --- CORES PARA O OUTPUT ---
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# --- FUNÇÃO DE TESTE ---
# Executa um comando, imprime o comando e seu resultado.
run_test() {
    echo -e "${YELLOW}--------------------------------------------------${NC}"
    echo -e "${YELLOW}TESTE:${NC} $1"
    echo -e "${YELLOW}COMANDO:${NC} $2"
    echo "--- SAÍDA ---"
    eval $2 # Executa o comando
    echo -e "---------------\n"
    # Pausa para o usuário poder ler a saída.
    read -p "Pressione Enter para continuar..." -r
}

# --- PREPARAÇÃO DO AMBIENTE ---
echo -e "${GREEN}=== INICIANDO AMBIENTE DE TESTE ===${NC}"

# 1. Compilar o programa
echo "Compilando o projeto com 'make'..."
make clean
make
if [ ! -f ./gbv ]; then
    echo -e "${RED}ERRO: Falha na compilação. Verifique seu código e Makefile.${NC}"
    exit 1
fi
echo "Compilação concluída com sucesso."

# 2. Criar arquivos de teste
echo "Criando arquivos de texto para os testes..."
echo "Este é o conteúdo do primeiro arquivo de teste." > arquivo1.txt
echo "Este é o segundo arquivo, um pouco maior para testar a remoção e visualização." > arquivo2.txt
echo "terceiro arquivo" > arquivo3.txt
touch vazio.txt # Arquivo vazio
head -c 1024 /dev/urandom > arquivo_grande.bin # Arquivo binário de 1KB

# 3. Definir nome da biblioteca
LIB_NAME="minhabiblioteca.gbv"

# Limpa a biblioteca de testes anteriores, se existir.
rm -f $LIB_NAME

echo -e "${GREEN}=== AMBIENTE PRONTO. INICIANDO TESTES ===${NC}"


# --- CASOS DE TESTE ---

# 1. Testes de Argumentos e Erros
run_test "Execução sem argumentos" "valgrind --leak-check=full --track-origins=yes ./gbv"
run_test "Execução com poucos argumentos" "valgrind --leak-check=full --track-origins=yes ./gbv -a"
run_test "Execução com opção inválida" "valgrind --leak-check=full --track-origins=yes ./gbv -x $LIB_NAME"

# 2. Adição de Arquivos (-a)
run_test "Adicionar um arquivo pela primeira vez (cria a biblioteca)" "valgrind --leak-check=full --track-origins=yes ./gbv -a $LIB_NAME arquivo1.txt"
run_test "Listar conteúdo após primeira adição" "valgrind --leak-check=full --track-origins=yes ./gbv -l $LIB_NAME"
run_test "Adicionar múltiplos arquivos de uma vez" "valgrind --leak-check=full --track-origins=yes ./gbv -a $LIB_NAME arquivo2.txt arquivo3.txt"
run_test "Listar conteúdo após múltiplas adições" "valgrind --leak-check=full --track-origins=yes ./gbv -l $LIB_NAME"
run_test "Adicionar um arquivo que não existe" "valgrind --leak-check=full --track-origins=yes ./gbv -a $LIB_NAME arquivo_inexistente.txt"
run_test "Adicionar um arquivo vazio" "valgrind --leak-check=full --track-origins=yes ./gbv -a $LIB_NAME vazio.txt"
run_test "Listar para verificar arquivo vazio" "valgrind --leak-check=full --track-origins=yes ./gbv -l $LIB_NAME"

# 3. Substituição de Arquivo
run_test "Substituir um arquivo existente (adicionar novamente)" "valgrind --leak-check=full --track-origins=yes ./gbv -a $LIB_NAME arquivo1.txt"
echo "Verifique se a data de 'arquivo1.txt' foi atualizada na listagem abaixo."
run_test "Listar para verificar a substituição" "valgrind --leak-check=full --track-origins=yes ./gbv -l $LIB_NAME"

# 4. Remoção de Arquivos (-r)
run_test "Remover um arquivo do meio da lista ('arquivo2.txt')" "valgrind --leak-check=full --track-origins=yes ./gbv -r $LIB_NAME arquivo2.txt"
run_test "Listar para verificar a remoção" "valgrind --leak-check=full --track-origins=yes ./gbv -l $LIB_NAME"
run_test "Remover um arquivo que não existe na biblioteca" "valgrind --leak-check=full --track-origins=yes ./gbv -r $LIB_NAME arquivo_inexistente.txt"
run_test "Adicionar arquivo de volta para próximos testes" "valgrind --leak-check=full --track-origins=yes ./gbv -a $LIB_NAME arquivo2.txt"
run_test "Remover o primeiro arquivo da lista ('arquivo1.txt')" "valgrind --leak-check=full --track-origins=yes ./gbv -r $LIB_NAME arquivo1.txt"
run_test "Listar para verificar remoção do primeiro" "valgrind --leak-check=full --track-origins=yes ./gbv -l $LIB_NAME"
run_test "Remover o último arquivo da lista ('arquivo2.txt')" "valgrind --leak-check=full --track-origins=yes ./gbv -r $LIB_NAME arquivo2.txt"
run_test "Listar para verificar remoção do último" "valgrind --leak-check=full --track-origins=yes ./gbv -l $LIB_NAME"

# 5. Visualização de Arquivos (-v)
# Nota: A visualização é interativa, o script não pode testar a navegação.
# O usuário deverá testar manualmente as teclas 'n', 'p' e 'q'.
echo -e "${YELLOW}--------------------------------------------------${NC}"
echo -e "${YELLOW}TESTE:${NC} Visualização de arquivo (teste interativo)"
echo "O próximo teste irá iniciar o modo de visualização."
echo "Por favor, teste as seguintes funcionalidades:"
echo "  - Pressione 'n' para ver o próximo bloco (se houver)."
echo "  - Pressione 'p' para ver o bloco anterior (se houver)."
echo "  - Pressione 'q' para sair."
echo -e "${YELLOW}COMANDO:${NC} ./gbv -v $LIB_NAME arquivo3.txt"
valgrind --leak-check=full --track-origins=yes ./gbv -v $LIB_NAME arquivo3.txt
echo -e "---------------\n"

# 6. Testes com arquivo maior
run_test "Adicionar um arquivo binário maior" "./gbv -a $LIB_NAME arquivo_grande.bin"
run_test "Listar para verificar arquivo grande" "./gbv -l $LIB_NAME"
run_test "Remover arquivo grande" "./gbv -r $LIB_NAME arquivo_grande.bin"
run_test "Listar após remoção do arquivo grande" "./gbv -l $LIB_NAME"


# --- LIMPEZA DO AMBIENTE ---
echo -e "${GREEN}=== FIM DOS TESTES. LIMPANDO AMBIENTE ===${NC}"
rm -f arquivo1.txt arquivo2.txt arquivo3.txt vazio.txt arquivo_grande.bin
rm -f $LIB_NAME
make clean
echo "Arquivos temporários removidos."
