
#
#      Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
#      All Rights Reserved
#
#      Note: Bell Laboratories was previously part of Lucent Technologies, later part
#      of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
#      assigned by AT&T to its successors. This license is granted by Nokia solely to
#      the extent of its right to do so.
#
# If you are using a 6300 or 7300, you should use 
# CFLAGS=-O -DPC7300
# If you are running Xenix, you should use
# CFLAGS=-M0 -Mm -compat
# and remember to modify FLAGS in bin/awkcc.sh.
CC=/bin/cc
CFLAGS=-O
PWD=/u2/src/personal/rbp/awkcc/src
TARGBIN=$(PWD)/bin
TARGSH=$(PWD)/bin
TARGLIB=$(PWD)/bin
TARGHDR=$(PWD)/include

install: others $(TARGBIN)/awkcc.sh

others:
#	(cd ./lib;make install CC=$(CC) CFLAGS=$(CFLAGS) TARGLIB=$(TARGLIB) TARGHDR=$(TARGHDR))
#	(cd ./awkcc;make install CC=$(CC) CFLAGS=$(CFLAGS) TARGBIN=$(TARGBIN))

$(TARGBIN)/awkcc.sh: makefile awkcc/awkcc.sh.basic
	echo "CC=$(CC)" > $(TARGSH)/awkcc.sh
	echo "HEADERDIR=$(TARGHDR)" >> $(TARGSH)/awkcc.sh
	echo "AWKLIB=$(TARGLIB)/libAWK.a" >> $(TARGSH)/awkcc.sh
	cat awkcc/awkcc.sh.basic >> $(TARGSH)/awkcc.sh
	chmod 755 $(TARGSH)/awkcc.sh
	

clean: install
	(cd ./lib;make clean CC=$(CC) CFLAGS=$(CFLAGS) TARGLIB=$(TARGLIB) TARGHDR=$(TARGHDR))
	(cd ./awkcc;make clean CC=$(CC) CFLAGS=$(CFLAGS) TARGBIN=$(TARGBIN))
