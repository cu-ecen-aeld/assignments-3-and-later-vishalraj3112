CC=$(CROSS_COMPILE)gcc
CFLAGS=-Werror -Wall -g -pthread

all: aesdsocket
default: aesdsocket

aesdsocket: aesdsocket.o
	$(CC) -o aesdsocket aesdsocket.o $(CFLAGS)	

appl=aesdsocket

.PHONY : clean
clean :
	rm -f $(appl) *.o

#valgrind --leak-check=yes -s --track-origins=yes ./aesdsocket
#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./aesdsocket
