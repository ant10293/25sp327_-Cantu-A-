CC=gcc
CFLAGS=-g -Wall -Wextra
TARGET1=compare_static
TARGET2=compare_dynamic

.PHONY: all clean compare

all: $(TARGET1) $(TARGET2)

compare.o: compare.c
	$(CC) $(CFLAGS) -c compare.c

$(TARGET1): compare.o
	$(CC) -static -o $(TARGET1) compare.o

$(TARGET2): compare.o
	$(CC) -o $(TARGET2) compare.o

clean:
	rm -fv *.o $(TARGET1) $(TARGET2)

compare:
	@echo "$(TARGET1):"
	@ls -lh $(TARGET1)
	@echo "$(TARGET2):"
	@ls -lh $(TARGET2)
	@echo "./$(TARGET1): $$(./$(TARGET1) > /dev/null 2>&1 && echo Success!)"
	@echo "./$(TARGET2): $$(./$(TARGET2) > /dev/null 2>&1 && echo Success!)"
