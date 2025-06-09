CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
BIN_DIR = bin

SRC = main.c \
      $(SRC_DIR)/equipement.c \
      $(SRC_DIR)/reseau.c

OBJ = $(patsubst %.c,$(BIN_DIR)/%.o,$(notdir $(SRC)))
EXEC = $(BIN_DIR)/reseauSimu

all: $(BIN_DIR) $(EXEC)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/main.o: main.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

TEST_EXEC = $(BIN_DIR)/test_affichage

tests: $(BIN_DIR) $(TEST_EXEC)

$(TEST_EXEC): tests/test_affichage.c $(BIN_DIR)/reseau.o $(BIN_DIR)/equipement.o
	$(CC) $(CFLAGS) -o $@ $^

run-tests: tests
	./$(TEST_EXEC)

clean:
	rm -f $(BIN_DIR)/*.o $(EXEC) $(TEST_EXEC)

run: all
	./$(EXEC)
