#	$NetBSD$
#
TARGET	= loadbsd

CC	= gcc
CFLAGS	= -D_STANDALONE -I./include -O -fomit-frame-pointer -msmall-code
LDFLAGS	= -noixemul
LDLIBS	=

OBJS	= loadbsd.o loadfile.o loadfile_aout.o loadfile_elf32.o getopt.o

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
