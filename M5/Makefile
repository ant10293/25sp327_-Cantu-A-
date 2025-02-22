CC=gcc
CFLAGS=-Wall -Wextra

all: system_info

system_info: system_info.c
	$(CC) $(CFLAGS) system_info.c -o system_info

clean:
	rm -f system_info

.PHONY: all clean
