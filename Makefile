PROJ = jvm
CC = gcc
CFLAGS =  -Iinclude \
		  -g \
	      -DDEBUG #-m32

LDFLAGS = -pthread
SRC = $(wildcard *.c) $(wildcard src/*.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

$(PROJ) : $(OBJ)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

.PHONEY:clean $(PROJ)
clean:
	@rm -rvf $(PROJ) $(TEST) $(OBJ)
