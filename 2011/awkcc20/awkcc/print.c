/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <stdio.h>
#include "awkcc.h"

decpfnum(s, num)
uchar	*s;
int	*num;
{
	uchar	*format;

	format=s;
	getnextfmt(format, num);
	while (getnextfmt((uchar *) NULL, num)>=0)
		;
}

decpf(e, num)
Nodep	e;
int	*num;
{
	int	i;
	uchar	*format;
	Nodep	tmp1;

	tmp1=e->LEFT;
	format=tmp1->s;
	for (tmp1=tmp1->nnext; tmp1; tmp1=tmp1->nnext) {
		if ((i=getnextfmt(format, num))<0) {
			fprintf(stderr, "AWKCC WARNING: Too many args to printf/sprintf.\n");
			eprint();
		}
		tmp1->w=i;
		if (EVALORD_TEMPSTR(tmp1)) tmp1->flags|=FIXORD;
		format=(uchar *) NULL;
	}
	if (getnextfmt(format, num)>=0)
		ER "not enough args to printf" SX;
}

getnextfmt(str, num)
uchar	*str;
int	*num;
{
	static uchar	*internal;
	int	c;

	if (str) {
		internal=str;
		*num=0;
	}
	while ((c=(*internal)) && c!='%') {
		++internal;
		++*num;
	}
	if (!c) return(-1);
	if (c=='%' && *(internal+1)=='%') {
		internal=internal+2;
		++*num;
		return(getnextfmt((uchar *)NULL, num));
	}
	c=(*++internal);
	if (c=='-') c=(*++internal);
	if (c>='0' && c<='9') {
		(*num)+=atoi(internal);
		while (c>='0' && c<='9')
			c=(*++internal);
	}
	if (c=='.') c=(*++internal);
	if (c>='0' && c<='9') {
		(*num)+=atoi(internal);
		while (c>='0' && c<='9')
			c=(*++internal);
	}
	/* "long" specification has a null effect, but is legal. */
	if (c=='l') c=(*++internal);
	switch(c) {
	case 'c':	return(YINT);
	case 'd':	return(YINT);
	case 'e':	return(YNUM);
	case 'f':	return(YNUM);
	case 'g':	return(YNUM);
	case 'o':	return(YINT);
	case 'x':	return(YINT);
	case 's':	return(YSTR);
	default:
		ER "Illegal printf format %%%c (use %%%% to print a %% sign)", (char)c SX;
	}
	return(0);
}
