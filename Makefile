CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = main.c \
      src/graphe.c \
      src/ip_adresses.c \
      src/mac_addresses.c \
      src/station.c \
      src/switch.c

OBJ = $(SRC:.c=.o)
EXEC = reseauSimu

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJ) $(EXEC)

run: all
	./$(EXEC)
