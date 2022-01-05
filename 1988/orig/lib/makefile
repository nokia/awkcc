
#
#      Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
#      All Rights Reserved
#
#      Note: Bell Laboratories was previously part of Lucent Technologies, later part
#      of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
#      assigned by AT&T to its successors. This license is granted by Nokia solely to
#      the extent of its right to do so.
#
SOURCES= misc.o \
	lines.o \
	make_str.o \
	copystr.o \
	index.o \
	xprintf.o \
	isreg.o \
	awkrun.o \
	awkmod.o \
	assignvars.o \
	awkerr.o \
	clear_htb.o \
	free_array.o \
	clearctest.o \
	delete.o \
	docomp.o \
	docompN.o \
	regexp.o \
	newb.o \
	awkparse.o \
	filedrop.o \
	fileget.o \
	getd.o \
	getrec.o \
	gsub.o \
	n_insert.o \
	s_insert.o \
	insert.o \
	make_array.o \
	make_buckets.o \
	make_elem.o \
	make_var.o \
	member.o \
	number.o \
	opennext.o \
	pipeget.o \
	printD0.o \
	print_var.o \
	resize.o \
	rgetrec.o \
	setctest.o \
	split.o \
	split2.o \
	split3.o \
	splitrec.o \
	strsplitrec.o \
	resplitrec.o \
	sresize.o \
	substr.o

libAWK.a: $(SOURCES)
	ar u libAWK.a $(SOURCES)

$(TARGLIB)/libAWK.a: libAWK.a
	rm -f $(TARGHDR)/*.h
	cp *.h $(TARGHDR)
	chmod 644 $(TARGHDR)/*.h
	cp libAWK.a $(TARGLIB); 
	-test -f /usr/bin/ranlib && ranlib $(TARGLIB)/libAWK.a

install: $(TARGLIB)/libAWK.a

clean: install
	rm -f *.o libAWK.a
