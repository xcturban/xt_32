SRC=$(wildcard *.c)
SRC+=$(wildcard core/*.c)
SRC+=$(wildcard ui/*.c)
SRC+=$(wildcard calllog/*.c)
SRC+=$(wildcard log/*.c)
SRC+=$(wildcard audio/*.c)
SRC+=$(wildcard call/*.c)
SRC+=$(wildcard broadcast/*.c)
SRC+=$(wildcard gpio/*.c)
SRC+=$(wildcard setting/*.c)
SRC+=$(wildcard sip/*.c)
SRC+=$(wildcard common/*.c)
SRC+=$(wildcard win/*.c)
SRC+=$(wildcard video/*.c)
SRC+=$(wildcard watchdog/*.c)


OBJ=$(patsubst %.c, %.o, $(SRC))

TARGET=ncs

CFLAGS=-I./ -I./core -I./win -I./gpio -I./common -I./ui -I./calllog -I./log -I./audio -I./call -I./broadcast -I./setting -I./sip -I./video -I./watchdog
LDFLAGS=-pthread -lm -lcommon_lib -lrt -lmediastreamer -lortp -lncs_event -liconv -lpng -lfreetype -lid3tag -lecho_hisi -lminigui_ths
#STATIC_LIB=$(STAGING_DIR)/usr/lib/libminigui_ths.a
$(TARGET):$(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o  $@ #$(STATIC_LIB)

.c.o:
	$(CC) $(CFLAGS) -c -o  $@ $<
