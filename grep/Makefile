CFLAGS = -Wall -Werror -Wextra -std=c11
CC = gcc $(CFLAGS)

SRC_DIR = .
BUILD_DIR = .
TESTS_DIR = ./tests
TEST = $(TESTS_DIR)/test.sh

GREP = $(BUILD_DIR)/my_grep


all: my_grep

my_grep:
	$(CC) $(SRC_DIR)/my_grep_func.c $(SRC_DIR)/my_grep.c -o $(GREP)

debug:
	rm -f $(GREP)
	gcc $(SRC_DIR)/my_grep_func.c $(SRC_DIR)/my_grep.c -o $(GREP)

test:
	bash $(TEST)

clean:
	rm $(GREP)

rebuild: clean all