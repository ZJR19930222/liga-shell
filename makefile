## build liga-shell
hd=liga.h
clg=/usr/bin/clang
ifeq (,$(wildcard $(clg)))
    CC=gcc
else
    CC=clang
endif
liga-shell:liga-shell.c estring.o variable.o builtin.o process.o
	$(CC) liga-shell.c estring.o variable.o builtin.o process.o -o liga-shell
process.o:$(hd) process.h process.c
	$(CC) -c process.c -o process.o
builtin.o:$(hd) builtin.h builtin.c
	$(CC) -c builtin.c -o builtin.o
variable.o:$(hd) variable.h variable.c
	$(CC) -c variable.c -o variable.o
estring.o:$(hd) estring.h estring.c
	$(CC) -c estring.c -o estring.o
clean:
	rm -f *.o
