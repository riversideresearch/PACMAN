CC=gcc
CFLAGS=-I../hammer/src
test:
	@$(CC) $(CFLAGS) -o runTests PACMAN.c `(pkg-config --libs --cflags libhammer)` `(pkg-config --libs --cflags glib-2.0)`

clean:
	@rm -r runTests
