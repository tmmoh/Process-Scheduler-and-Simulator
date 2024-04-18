CC = gcc
CFLAGS = -Wall -g -Wno-unknown-pragmas
LIB = -lm

SRC = src/main.c src/process.c src/queue.c src/roundrobin.c src/linkedlist.c src/memory.c
OBJ = $(SRC:.c=.o)
 
EXE = allocate

$(EXE): $(OBJ) 
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIB)

clean:
	rm -f $(OBJ) $(EXE)
