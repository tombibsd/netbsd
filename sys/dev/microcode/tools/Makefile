#	$NetBSD$

BIN=	bin2blob array2bin

all:	${BIN}

bin2blob:	bin2blob.c	
		cc -Wall -O2 -lz -o bin2blob bin2blob.c

array2bin:	array2bin.c	
		cc -Wall -O2 -lz -o array2bin array2bin.c

clean:
		rm -f ${BIN} *~
