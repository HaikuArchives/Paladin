#
# GNU getopt library
#

all: libgnugetopt.so.1 libgnugetopt.a

libgnugetopt.so.1: getopt.o getopt1.o
	${CC} ${CFLAGS} -shared -Wl,-soname,libgnugetopt.so.1 -o libgnugetopt.so.1 getopt.o getopt1.o

libgnugetopt.a: getopt.o getopt1.o
	@echo building static libgnugetopt library
	ar cq libgnugetopt.a `lorder getopt.o getopt1.o | tsort -q`
	ranlib libgnugetopt.a

getopt1.o: getopt1.c
	${CC} ${CFLAGS} -fPIC -o getopt1.o -c -s getopt1.c

getopt.o: getopt.c
	${CC} ${CFLAGS} -fPIC -o getopt.o -c -s getopt.c

clean:
	rm *.o *.so.1
