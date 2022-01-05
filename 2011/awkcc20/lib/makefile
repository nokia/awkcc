SOURCES= misc.o \
	lines.o \
	gsub.o \
	xprintf.o \
	awkrun.o \
	awkerr.o \
	comparisons.o \
	b.o \
	awkparse.o \
	files.o \
	getd.o \
	getrec.o \
	make_array.o \
	make_elem.o \
	make_str.o \
	make_var.o \
	opennext.o \
	D0.o \
	ctest.o \
	split.o \
	split2.o \
	split3.o \
	splitrec.o \
	strsplitrec.o \
	resplitrec.o \
	hash.o

libAWK.a: $(SOURCES) $(MALLOC)
	ar ru libAWK.a $(SOURCES) $(MALLOC)
	cp libAWK.a *.h ../../include/

$(OBJROOT)/$(TARGLIB)/libAWK.a: libAWK.a
	[ -d $(OBJROOT)/$(TARGHDR) ] ||  mkdir -p $(OBJROOT)/$(TARGHDR)
	[ -d $(OBJROOT)/$(TARGLIB) ] ||  mkdir -p $(OBJROOT)/$(TARGLIB)
	rm -f $(OBJROOT)/$(TARGHDR)/*.h
	cp *.h $(OBJROOT)/$(TARGHDR)
	chmod 644 $(OBJROOT)/$(TARGHDR)/*.h
	cp libAWK.a $(OBJROOT)/$(TARGLIB); 
	-test -f /usr/bin/ranlib && ranlib $(TARGLIB)/libAWK.a

install: $(OBJROOT)/$(TARGLIB)/libAWK.a

clean:
	rm -f $(SOURCES) libAWK.a
