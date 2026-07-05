CFLAGS = -Wall -Werror -Wextra -std=c11
CC = gcc $(CFLAGS)

SRC_DIR = .
BUILD_DIR = .
TESTS_DIR = ./tests
TEST = $(TESTS_DIR)/test.sh

CAT = $(BUILD_DIR)/my_cat


all: my_cat

my_cat:
	$(CC) $(SRC_DIR)/my_cat_func.c $(SRC_DIR)/my_cat.c -o $(CAT)

test:
	bash $(TEST)

clean:
	rm $(CAT)

rebuild: clean all