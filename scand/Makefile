CC=mipsel-linux-uclibc-gcc
STRIP=mipsel-linux-uclibc-strip


CFLAGS = -Wall -O2
LIBS = -lm
PICFLAG=-fPIC

PROGS = scanc

all: $(PROGS)

%: %.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@
%.o: %.c 
	$(CC) $(CFLAGS) -c $<
%.so: %.c
	$(CC) $(CFLAGS) $(PICFLAG) -c -o $@ $<


httpd: scanc.o


clean:
	rm -f $(PROGS) *.o
