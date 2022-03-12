CC = $(CROSS_COMPILE)gcc

#Flags
CFLAGS = -Wall -Werror -g

LDFLAG = -pthread -lrt 

#Source
SRCS = circular_buffer_test.c

#Object
OBJS = $(SRCS:.c=.o)

# Target
TARGET = circular_buffer_test

all: $(TARGET)
default: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAG) $^ -o $@


.PHONY: clean

clean:
	$(RM) $(TARGET) *.o

#valgrind --leak-check=yes -s --track-origins=yes ./circular_buffer_test
#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./circular_buffer_test
