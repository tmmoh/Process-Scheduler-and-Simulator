CC = gcc
CFLAGS = -Wall -g
LIB = -lm

SRC = src/main.c src/process.c src/queue.c src/roundrobin.c src/linkedlist.c src/memory.c
OBJ = $(SRC:.c=.o)
 
EXE = allocate

$(EXE): $(OBJ) 
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIB)

format:
	clang-format -style=file -i src/*.c src/*.h

clean:
	rm -f $(OBJ) $(EXE)