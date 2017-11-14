SRC=$(wildcard *.c) 
OBJ=$(patsubst %.c, %.o, $(SRC))

TARGET=libcommon_lib.so

CFLAGS=-I./
LDFLAGS=--shared -fPIC

$(TARGET):$(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
