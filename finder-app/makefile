CC=$(CROSS_COMPILE)gcc
CFLAGS=-Werror -Wall -g

all: writer

writer: writer.o
	$(CC) -o writer writer.o	

appl=writer

.PHONY : clean
clean :
	rm -f $(appl) *.o

