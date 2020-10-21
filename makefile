## build liga-shell
hd=liga.h
all:liga-shell
liga-shell:liga-shell.c estring.o variable.o builtin.o process.o
	clang liga-shell.c estring.o variable.o builtin.o process.o -o liga-shell
process.o:$(hd) process.h process.c
	clang -c process.c -o process.o
builtin.o:$(hd) builtin.h builtin.c
	clang -c builtin.c -o builtin.o
variable.o:$(hd) variable.h variable.c
	clang -c variable.c -o variable.o
estring.o:$(hd) estring.h estring.c
	clang -c estring.c -o estring.o
clean:
	rm -f *.o
