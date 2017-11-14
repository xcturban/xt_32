SRC=$(wildcard *.c) 
OBJ=$(patsubst %.c, %.o, $(SRC))

TARGET=libncs_event.so

CFLAGS=-I./ 
LDFLAGS=--shared -fPIC

$(TARGET):$(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
