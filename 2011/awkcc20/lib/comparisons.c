/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

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

docompN(v1, num, op, rev)
register VARP	v1;
double	num;
register int	op;
register int	rev;
{
	char	numstr[CONVSIZ];
	double	ans;
	extern char	*OFMT;	/* new */

	if (MUSTSTR(v1->cur))
		goto STRCMP;
	if (!DIDCHECK(v1->cur))
		v1->cur|=isitanum(S(v1));
	if (ISNUM(v1->cur)) {
		if (!NUMAVAIL(v1->cur)) {
			v1->num=atof(S(v1));
			v1->cur|=NUMFLD;
		}
		ans=v1->num-num;
		goto DONE;
	}
	if (!STRAVAIL(v1->cur))
		ToStr(v1);
	STRCMP:;
	sprintf(numstr, OFMT, num);
	ans=strcmp(S(v1), numstr);
	DONE:;
	switch(op) {
	case LT:	return(rev?ans>0:ans<0);
	case LTE:	return(rev?ans>=0:ans<=0);
	case GT:	return(rev?ans<0:ans>0);
	case GTE:	return(rev?ans<=0:ans>=0);
	case EQ:	return(ans==0);
	case NEQ:	return(ans!=0);
	default: awkerr("unimplemented comparison op");
	}
	return(-1);
}
