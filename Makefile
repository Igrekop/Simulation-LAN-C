CC = gcc
CFLAGS = -Wall -Werror -Wextra -g -Iinclude
SRC_DIR = src
BIN_DIR = bin

SRC = main.c \
      $(SRC_DIR)/equipement.c \
      $(SRC_DIR)/trame.c \
      $(SRC_DIR)/commutation.c \
      $(SRC_DIR)/stp.c \
      $(SRC_DIR)/reseau.c \
      $(SRC_DIR)/affichage.c

SRCS_ROOT = main.c
SRCS_IN_DIR = equipement.c trame.c commutation.c stp.c reseau.c affichage.c

OBJS_ROOT = $(patsubst %.c,$(BIN_DIR)/%.o,$(SRCS_ROOT))
OBJS_IN_DIR = $(patsubst %.c,$(BIN_DIR)/%.o,$(SRCS_IN_DIR))
OBJS = $(OBJS_ROOT) $(OBJS_IN_DIR)

EXEC = $(BIN_DIR)/simulateur_reseau

all: $(EXEC)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/main.o: main.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

TEST_EXEC = $(BIN_DIR)/run_tests

tests: $(TEST_EXEC)

$(TEST_EXEC): tests/test_runner.c $(OBJS) | $(BIN_DIR) 
	$(CC) $(CFLAGS) -o $@ $^

run-tests: tests
	@echo "Exécution des tests..."
	./$(TEST_EXEC)

clean:
	@echo "Nettoyage des fichiers compilés..."
	@rm -f $(BIN_DIR)/*.o $(EXEC) $(TEST_EXEC) 
	@rmdir $(BIN_DIR) 2>/dev/null || true

run: all
	@echo "Lancement du simulateur réseau..."
	./$(EXEC) reseau_config.txt

.PHONY: all clean run tests run-tests
