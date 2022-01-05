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

all: awkcc awkcc.sh
	cp awkcc$(EXEEXT) ../..
	cp awkcc.sh ../..

awkcc: $(TARG)
	$(CC) $(CFLAGS) -o awkcc $(TARG)

awkcc.sh: awkcc.sh.basic
	echo "CC=$(CC)" > awkcc.sh
	echo HEADERDIR=/opt/exp/lib/awkcc/include >> awkcc.sh
	echo AWKLIB=/opt/exp/lib/awkcc/libAWK.a >> awkcc.sh
	cat awkcc.sh.basic >> awkcc.sh

$(OBJROOT)/$(TARGBIN)/awkcc: awkcc
	[ -d $(OBJROOT)/$(TARGBIN) ] || mkdir -p $(OBJROOT)/$(TARGBIN)
	cp awkcc $(OBJROOT)/$(TARGBIN)
	strip $(OBJROOT)/$(TARGBIN)/awkcc

install: $(OBJROOT)/$(TARGBIN)/awkcc

clean:
	rm -f $(TARG) awkcc y.tab.h
