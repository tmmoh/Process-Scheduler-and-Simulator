CC = gcc
CFLAGS = -Wall -g
LIB = 

SRC = src/main.c
OBJ = $(SRC:.c=.o)
 
EXE = allocate

$(EXE): $(OBJ) 
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIB)

clean:
	rm -f $(OBJ) $(EXE)
