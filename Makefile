# macros
CC = gcc217
CFLAGS =

all: testsymtablelist testsymtablehash
clobber: clean
	rm -f *~ \#*\#
clean:
	rm -f testsymtablelist testsymtablehash *.o

testsymtablelist: testsymtable.o symtablelist.o
	$(CC) testsymtable.o symtablelist.o -o testsymtablelist
testsymtablehash: testsymtable.o symtablehash.o
	$(CC) testsymtable.o symtablehash.o -o testsymtablehash
testsymtable.o: testsymtable.c 
symtablelist.o: symtable.h symtablelist.c
symtablehash.o: symtable.h symtablehash.c

