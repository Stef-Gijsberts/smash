CFLAGS=

CC=gcc
RM=rm -f

smash: smash.o
	$(CC) $(CFLAGS) -o smash smash.o

install: smash
	install smash /usr/local/bin/

.PHONY: clean

clean:
	rm -f *.o smash
