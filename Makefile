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

install:
	/usr/bin/install -m 755 -D action-btn-pressed /usr/sbin/action-btn-pressed
	/usr/bin/install -m 755 -D action-disk-plugged /usr/sbin/action-disk-plugged
	/usr/bin/install -m 755 -D action-disk-unplugged /usr/sbin/action-disk-unplugged
	/usr/bin/install -m 755 -D action-part-over-usage /usr/sbin/action-part-over-usage
	/usr/bin/install -m 755 -D mxhtspd-handle-disk-plugged /usr/sbin/mxhtspd-handle-disk-plugged
	/usr/bin/install -m 755 -D mxhtspd-handle-disk-unplugged /usr/sbin/mxhtspd-handle-disk-unplugged
	/usr/bin/install -m 755 -D mxhtspd-remove-disk /usr/sbin/mxhtspd-remove-disk
	/usr/bin/install -m 755 -D mxhtspd-setled /usr/sbin/mxhtspd-setled
	/usr/bin/install -m 755 -D mx_hotswapd.sh /usr/sbin/mx_hotswapd.sh
	/usr/bin/install -m 755 -D mxhtspd /usr/sbin/mxhtspd
	/usr/bin/install -m 644 -D libmxhtsp.so /usr/lib/libmxhtsp.so
	/usr/bin/install -m 644 -D 96-moxa-disk.rules /etc/udev/rules.d/96-moxa-disk.rules
	/usr/bin/install -m 644 -D mx_hotswapd.service /lib/systemd/system/mx_hotswapd.service
