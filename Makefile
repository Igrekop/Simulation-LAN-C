CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = main.c \
      src/equipement.c \
      src/reseau.c \

OBJ = $(patsubst %.c,bin/%.o,$(notdir $(SRC)))

EXEC = bin/reseauSimu

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

bin/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f bin/*.o $(EXEC)

run: all
	./$(EXEC)

