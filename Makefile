CC = gcc
CFLAGS = -Wall -g
LIB = -lm

SRC = src/main.c src/process.c src/queue.c src/roundrobin.c
OBJ = $(SRC:.c=.o)
 
EXE = allocate

$(EXE): $(OBJ) 
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIB)

clean:
	rm -f $(OBJ) $(EXE)
