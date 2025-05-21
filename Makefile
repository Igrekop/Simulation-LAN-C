CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = main.c \
      src/graphe.c \
      src/ip_adresses.c \
      src/mac_addresses.c \
      src/station.c \
      src/switch.c

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

