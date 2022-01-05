
# The beginning of this Makefile is the part that varies per-package
PKG = awkcc
PKGBALL = $(PKG).cpiogz
SRCDIR = awkcc20
PREFIX = /opt/exp

# Uncomment WORLDWIDE on the next line if this package should go into 
# "Worldwide Exptools" - these tools are made available on the public
# (external) internet, so only non-proprietary tools with open-source 
# licenses should set this.  In addition, the Exptools central 
# administrator (exptools@lucent.com) needs to mark the package as
# worldwide.
#WORLDWIDE=-worldwide

# VERSION is only really needed when building a package "worldwide" 
VERSION=1.0

# DIR_SUFFIX is normally blank, but is set if the tools are installed
# somewhere other than /opt/exp.  For example, /gnu, /ast, or /perl
DIR_SUFFIX=

# for all types use TYPES = `machines gnutypes|xargs|tr ' ' ','`
#TYPES=sparc-sun-solaris,sparc-sun-sunos
TYPES = `machines types|xargs|tr ' ' ','`

awkcc.cpiogz: awkcc
	PATH=/opt/exp/gnu/bin:$$PATH strip awkcc$(EXEEXT)
	mv include/libAWK.a .
	ranlib libAWK.a
	find awkcc.sh awkcc$(EXEEXT) libAWK.a include -print | cpio -ocv | gzip > awkcc.cpiogz

awkcc: force
	mkdir -p include
	cd awkcc20/lib; \
	export CC; CC=$(ANSICC); \
	case $(TYPE) in \
	isolaris ) \
		: use gnu yacc instead of /usr/ccs/bin/ to fix tests on isolaris; \
		PATH="/opt/exp/gnu/bin:$$PATH";; \
	idarwin ) CC="gcc -m32 -D_FORTIFY_SOURCE=0";; \
	darwin_gcc2 ) CC="gcc2";; \
	cygwin_gcc2 ) CC="gcc-2";; \
	hppa_gcc2 | isolaris_gcc2 ) CC=/opt/exp/old/bin/gcc;; \
	esac; \
	$(MAKE) -f makefile; \
	cd ../awkcc; \
	$(MAKE) -f makefile

force:
	rm -rf awkcc.zip include awkcc20/awkcc/awkcc \
		awkcc20/awkcc/awkcc.sh awkcc20/lib/libAWK.a \
		awkcc20/lib/*.o
unpack:
	@set -ex; \
	if [ -n "$(TYPES)" ]; then \
		for file in $(PKGBALL).*; do \
			T=`echo $$file | sed -e "s/$(PKGBALL).//"`; \
			mkdir -p ins/$$T; \
			cd ins/$$T; \
			gzcat < ../../$$file | cpio -ivdum; \
			cd ../..; \
			rm -f $$file; \
		done; \
	fi

