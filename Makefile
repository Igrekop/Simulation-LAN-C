CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
BIN_DIR = bin

SRC = main.c \
      $(SRC_DIR)/equipement.c \
      $(SRC_DIR)/reseau.c

OBJ = $(patsubst %.c,$(BIN_DIR)/%.o,$(notdir $(SRC)))
EXEC = $(BIN_DIR)/reseauSimu

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

TEST_EXEC = bin/test_affichage

tests: $(TEST_EXEC)

$(TEST_EXEC): tests/test_affichage.c bin/reseau.o bin/equipement.o
	$(CC) $(CFLAGS) -o $@ $^

run-tests: tests
	./$(TEST_EXEC)

clean:
	rm -f $(BIN_DIR)/*.o $(EXEC)

run: all
	./$(EXEC)
