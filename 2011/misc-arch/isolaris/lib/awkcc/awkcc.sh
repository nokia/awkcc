CC=${CC:-cc}
HEADERDIR=/opt/exp/lib/awkcc/include
AWKLIB=/opt/exp/lib/awkcc/libAWK.a
MATHLIB=-lm
CCPARMS=$3
# For Xenix, use
# FLAGS="-M0 -Mm -compat"
# if you have kpv's malloc package, add -DKMALLOC to get the -m option
FLAGS="-O $CCPARMS"

# Note that in the uts machines the crt0.o is in /native or ....u370/lib/crt0.o
# On most machines it is in /lib/crt0.o
# Thus the setting below should not create problems in the  UTS environment.
# Therefore, set it to /usr/ccs/lib/crt0.o, only if file exists.
CRT=/lib/crt0.o
[ -s /usr/ccs/lib/crt0.o ] && CRT=/usr/ccs/lib/crt0.o

set -e
case $4 in
	"0")	$CC -I$HEADERDIR $FLAGS -o $2 $1 $AWKLIB $MATHLIB 
		;;
	"1")	$CC -I$HEADERDIR $FLAGS -pg -o $2 $1 $AWKLIB $MATHLIB 
	     	echo "This executable will create a gprof file."
		;;
	"2")	$CC -I$HEADERDIR -S -O -o awker.s $1 
		as -J -o awker.o awker.s 
		ld -X $CRT -o $2 awker.o $AWKLIB $MATHLIB -lc 
		rm -f awker.s awker.o 
		;;
	*)	echo "ERROR" 
		exit 1
		;;
esac
exit 0
