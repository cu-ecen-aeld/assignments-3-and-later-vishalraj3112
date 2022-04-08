CROSS_COMPILE=

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

ifeq ($(LDFLAGS),)
	LDFLAGS= -lrt
endif

all: aesdsocket_new
default: aesdsocket_new

aesdsocket_new : aesdsocket_new.o
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o aesdsocket_new aesdsocket_new.o $(LDFLAGS)

aesdsocket_new.o : aesdsocket_new.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c aesdsocket_new.c $(LDFLAGS)

clean :
	rm -f *.o client
