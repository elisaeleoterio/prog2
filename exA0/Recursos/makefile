CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g
LDLIBS  = -lm
MAIN    = main
ENTREGA = $(MAIN)

# lista de arquivos de cabeçalho (a completar)
HDR = jukefila.h pedido.h

# lista de arquivos-objeto (a completar)
OBJ = jukefila.o pedido.o main.o 

# construir o executável
$(MAIN): $(MAIN).o $(OBJ)

# construir os arquivos-objeto (a completar)
$(MAIN).o: $(MAIN).c $(HDR)

# construir os TADs
jukefila.o: jukefila.c jukefila.h
fprio.o: pedido.c pedido.h

# executar
run: $(MAIN)
	./$(MAIN)

# testar no Valgrind
valgrind: $(MAIN)
	valgrind --leak-check=full --track-origins=yes ./$(MAIN)
