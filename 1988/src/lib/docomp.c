/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/

#include <stdio.h>
#include "ear.h"
	
extern void	ToStr();
double atof();

docomp(v1, v2, op)
register VARP	v1;
register VARP	v2;
register int	op;
{
	double	ans;

	if (MUSTSTR(v1->cur) || MUSTSTR(v2->cur))
		goto STRCMP;
	if (!DIDCHECK(v1->cur))
		v1->cur|=isitanum(S(v1));
	if (!DIDCHECK(v2->cur))
		v2->cur|=isitanum(S(v2));
	if (ISNUM(v1->cur) && ISNUM(v2->cur)) {
		if (!NUMAVAIL(v1->cur)) {
			v1->num=atof(S(v1));
			v1->cur|=NUMFLD;
		}
		if (!NUMAVAIL(v2->cur)) {
			v2->num=atof(S(v2));
			v2->cur|=NUMFLD;
		}
		ans=v1->num-v2->num;
		goto DONE;
	}
	STRCMP:;
	if (!STRAVAIL(v1->cur))
		ToStr(v1);
	if (!STRAVAIL(v2->cur))
		ToStr(v2);
	ans=strcmp(S(v1),S(v2));
	DONE:;
	switch(op) {
	case LT:	return(ans<0);
	case LTE:	return(ans<=0);
	case GT:	return(ans>0);
	case GTE:	return(ans>=0);
	case EQ:	return(ans==0);
	case NEQ:	return(ans!=0);
	default: awkerr("unimplemented comparison op");
	}
	return(-1);
}
