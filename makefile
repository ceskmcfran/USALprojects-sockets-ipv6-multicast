CC = gcc
CFLAGS = 
LIBS = 

PROGS = receptor emisor

all: ${PROGS}

receptor: receptor.o
	${CC} ${CFLAGS} -o $@ receptor.o ${LIBS}

difusor: difusor.o
	${CC} ${CFLAGS} -o $@ emisor.o ${LIBS}

clean:
	rm *.o ${PROGS}
