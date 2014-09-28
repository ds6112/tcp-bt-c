CC=gcc
CFLAGS=-g -Wall -std=c99
LDFLAGS= -lcrypto

BIN= rubt
SRC= main.c helper.c bencode.c sha1.c
OBJ=$(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o rubt $(OBJ)
clean:
	rm *.o $(BIN)