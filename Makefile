CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
BIN_DIR = bin

SRC = main.c \
      $(SRC_DIR)/equipement.c \
      $(SRC_DIR)/reseau.c \
      $(SRC_DIR)/trame.c

OBJ = $(patsubst %.c,$(BIN_DIR)/%.o,$(notdir $(SRC)))

EXEC = $(BIN_DIR)/reseauSimu

all: $(EXEC)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(EXEC): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/main.o: main.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

TEST_EXEC = $(BIN_DIR)/test_affichage

tests: $(TEST_EXEC)

$(TEST_EXEC): tests/test_affichage.c $(BIN_DIR)/reseau.o $(BIN_DIR)/equipement.o | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

run-tests: tests
	./$(TEST_EXEC)

clean:
	@echo "Nettoyage des fichiers compilés..."
	@rm -f $(BIN_DIR)/*.o $(EXEC) $(TEST_EXEC) # Supprime les .o et les exécutables
	@rmdir $(BIN_DIR) 2>/dev/null || true # Tente de supprimer le dossier bin s'il est vide, ignore l'erreur sinon

run: all
	./$(EXEC)

.PHONY: all clean run tests run-tests
