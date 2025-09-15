CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g
LDLIBS  = -lm
MAIN    = main
ENTREGA = $(MAIN)

# lista de arquivos de cabeçalho (a completar)
HDR = pelucia.h 

# lista de arquivos-objeto (a completar)
OBJ = pelucia.o main.o 

# construir o executável
$(MAIN): $(MAIN).o $(OBJ)

# construir os arquivos-objeto (a completar)
$(MAIN).o: $(MAIN).c $(HDR)

# construir os TADs
pelucia.o: pelucia.c pelucia.h

# executar
run: $(MAIN)
	./$(MAIN)

# testar no Valgrind
valgrind: $(MAIN)
	valgrind --leak-check=full --track-origins=yes ./$(MAIN)
