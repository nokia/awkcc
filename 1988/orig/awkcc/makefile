
#
#      Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
#      All Rights Reserved
#
#      Note: Bell Laboratories was previously part of Lucent Technologies, later part
#      of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
#      assigned by AT&T to its successors. This license is granted by Nokia solely to
#      the extent of its right to do so.
#
LIBS=-lm
YFLAGS=-d
TARG=	awk.g.o \
	awk.lx.o \
	main.o \
	getopt.o \
	prefuncall.o \
	comp.o \
	print.o \
	arrstack.o \
	node.o \
	walk.o \
	write.o \
	type.o \
	symtab.o \
	stack.o \
	tempvar.o \
	emitstmt.o \
	emitfunc.o \
	emitppmm.o \
	misc.o \
	fix.o \
	printop.o \
	emit.o

CSRCS=	awk.g.y \
	awk.lx.l \
	main.c \
	getopt.c \
	prefuncall.c \
	comp.c \
	print.c \
	arrstack.c \
	node.c \
	evalord.c \
	walk.c \
	write.c \
	type.c \
	symtab.c \
	stack.c \
	tempvar.c \
	emitstmt.c \
	emitfunc.c \
	emitppmm.c \
	misc.c \
	fix.c \
	printop.c \
	emit.c

HSRCS=	awkcc.h \
	funcs.h \
	parse.h \
	structs.h \
	y.tab.h \
	err.h \
	misc.h \
	tokens.h

awkcc: $(TARG)
	$(CC) $(CFLAGS) -o awkcc $(TARG)

$(TARGBIN)/awkcc: awkcc
	cp awkcc $(TARGBIN)
	strip $(TARGBIN)/awkcc

install: $(TARGBIN)/awkcc

clean: install
	rm -f *.o awkcc
