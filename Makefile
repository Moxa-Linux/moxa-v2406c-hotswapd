#DEBUG = -DDEBUG -g 
CFLAGS= -Wall $(DEBUG)
LDFLAGS= -L. -lmxhtsp -lpthread
CC=gcc
PROG = mxhtspd

LIB = libmxhtsp.so
LIB_FILE = mxhtsp_lx.c
LED_PROG = mxhtspd-setled

all: $(LIB) $(PROG)

$(LIB): $(LIB_FILE) 
	@$(CC) $(CFLAGS) -fPIC $(LIB_FILE) -c 
	@$(CC) $(CFLAGS) -shared $(LIB_FILE:.c=.o) -o $(LIB) -lc

$(PROG): $(PROG).c $(LIB) $(LED_PROG).c
	@$(CC) $(CFLAGS) $(LDFLAGS) $(PROG).c -o $(PROG)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(LED_PROG).c -o $(LED_PROG)

clean:
	@rm -f $(PROG) $(LIB) $(LED_PROG) mxhtsp_lx.o

mytest:
	@$(CC) $(CFLAGS) $(LDFLAGS) mytest.c -o mytest

