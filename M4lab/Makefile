CC=gcc
CFLAGS=-Wall -Wextra
CFLAGS_DEBUG=-g -ggdb -gdwarf-4 -O0
LIBS=-lcmocka

.PHONY: all clean input_main test_input

all: clean input_main test_input

clean:
	@echo "rm -fv input_main test_input *.o"
	@rm -fv input_main test_input *.o

input_main: input_main.o
	@echo "\nLink step to create input_main:"
	@echo "gcc -o input_main input_main.o"
	@$(CC) -o input_main input_main.o
	@echo "gcc $(CFLAGS_DEBUG) $(CFLAGS) -DUNIT_TEST -c input.c -o input.o"
	@$(CC) $(CFLAGS_DEBUG) $(CFLAGS) -DUNIT_TEST -c input.c -o input.o
	@echo "gcc $(CFLAGS) -c -o test_input.o test_input.c"
	@$(CC) $(CFLAGS) -c test_input.c -o test_input.o

test_input: test_input.o input.o
	@echo "\nLink step to create test_input:"
	@echo "gcc -o test_input test_input.o input.o -lcmocka"
	@$(CC) -o test_input test_input.o input.o $(LIBS)

input_main.o: input.c
	@echo "\ngcc $(CFLAGS) -c input.c -o input_main.o"
	@$(CC) $(CFLAGS) -c input.c -o input_main.o

input.o: input.c
	@$(CC) $(CFLAGS_DEBUG) $(CFLAGS) -DUNIT_TEST -c input.c -o input.o

test_input.o: test_input.c
	@$(CC) $(CFLAGS) -c test_input.c -o test_input.o


