CC=gcc
LINKER=gcc

# -I../librequest/include	Tells the compiler where to find header files
CFLAGS=-Wall -Wextra -O0 -I../librequest/include
LDFLAGS=-g

# -I../librequest/include	Tells the compiler where to find header files
# -L../librequest/lib		Tells the linker where to find librequest.a
# -lrequest					Links with librequest.a (prefix lib and suffix .a are implied)
# -lpthread					Link with POSIX threads
LIBS=-lpthread -L../librequest/lib/ -lrequest

OBJS=main.o http_proxy.o
TARGETS=http_proxy

all: $(TARGETS)

http_proxy: $(OBJS)
	$(LINKER) $(LINKER_FLAGS) $(OBJS) $(LIBS) -o http_proxy

main.o: main.c http_proxy.h
	$(CC) $(CFLAGS) -c main.c -o main.o 

http_proxy.o: http_proxy.c http_proxy.h
	$(CC) $(CFLAGS) -c http_proxy.c -o http_proxy.o

clean:
	rm -f $(OBJS) $(TARGETS)
