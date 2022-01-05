
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
#include "awkcc.h"

int	cfstkp;
uchar	*cfstk[MAX_LEVELS];	/* keep track of recursion 	*/

int	cbfstkp;
int	cbfstk[MAX_LEVELS];	/* keep track for "return"	*/

int	ccstkp;
int	ccstk[MAX_LEVELS];	/* keep track for "continue" 	*/

int	cbstkp;
int	cbstk[MAX_LEVELS];	/* keep track for "break" 	*/

int	currpush;

extern VARINFOP	*symtab;

VARINFOP
make_varinfo()
{
	VARINFOP	retval;
	int	i;

	retval=(VARINFOP)calloc(1, sizeof(VARINFO));
	retval->string=(uchar *)"";
	for (i=0; i<=currpush; i++) {
		(retval->cur)[i]=YUNK;
		(retval->rt)[i]=YUNK;
	}
	retval->exprs=(Nodep *)calloc(1, sizeof(Nodep));
	retval->ne=0;
	retval->atyped=0;
	retval->func=(uchar *)"";
	retval->type=(-1);
	return(retval);
}

vstkop(type, off)
int	type;
short	off;
{
	register int	i;
	register VARINFOP	tmp;
	int	save;

	switch(type) {
	case NULLIFY:
		for (i=0; tmp=symtab[i]; i++) {
			if (OPTVAR(tmp->type)) {
				(tmp->cur)[currpush]=YUNK;
				(tmp->rt)[currpush]=YUNK;
			}
		}
		break;
	case PUSH:
		currpush++;
		if (currpush>=MAX_LEVELS)
			ER "Too many nested statements" SX;
		for (i=0; tmp=symtab[i]; i++)
			if (OPTVAR(tmp->type)) {
				(tmp->cur)[currpush]=(tmp->cur)[currpush-1];
				(tmp->rt)[currpush]=(tmp->rt)[currpush-1];
			}
		break;
	case INVERT:
		for (i=0; tmp=symtab[i]; i++)
			if (OPTVAR(tmp->type)) {
				save=(tmp->cur)[currpush];
				(tmp->cur)[currpush]=(tmp->cur)[currpush-1];
				(tmp->cur)[currpush-1]=save;
				save=(tmp->rt)[currpush];
				(tmp->rt)[currpush]=(tmp->rt)[currpush-1];
				(tmp->rt)[currpush-1]=save;
			}
		break;
	case AFFECTF:
		for (i=0; tmp=symtab[i]; i++) 
			if (OPTVAR(tmp->type))
				resolve(tmp, currpush, off+cbfstk[cbstkp-1]);
		break;
	case AFFECT:
		i=off?cbstkp:ccstkp;
		if (i<=0)
			ER off?"Illegal continue":"Illegal break" SX;
		save=off?cbstk[cbstkp-1]:ccstk[ccstkp-1];
		for (i=0; tmp=symtab[i]; i++) 
			if (OPTVAR(tmp->type))
				resolve(tmp, currpush, save);
		break;
	case POP:
		currpush--;
		break;
	case POPR:
		currpush--;
		if (currpush<0)
			fprintf(stderr, "CURRPUSH <0 (%d)!\n", currpush);
		for (i=0; tmp=symtab[i]; i++)
			if (OPTVAR(tmp->type))
				resolve(tmp, currpush+1, currpush);
		break;
	default:
		fprintf(stderr, "operation %d.\n", type);
		ER "Invalid stk operation" SX;
	}
}

fstkop(type, str)
int	type;
uchar	*str;
{
	int	i;

	switch(type) {
	case PUSH:
		if (cfstkp>=MAX_LEVELS)
			ER "Too many nested function calls" SX;
		cfstk[cfstkp++]=str;
		break;
	case POP:
		--cfstkp;
		break;
	case FIND:
		for (i=0; i<cfstkp; i++)
			if (!strcmp(str, cfstk[i]))
				return(1);
		return (0);
		break;
	case CHECKOK:
		return (cfstkp<2);
		break;
	default:
		ER "Invalid fstk operation" SX;
		break;
	}
	return(0);
}

genstkop(type, stk)
int	type;
int	stk;
{
	int *rstk, *stkp;

	switch(stk) {
	case CBFSTACK:
		rstk=cbfstk;
		stkp= &cbfstkp;
		break;
	case CBSTACK:
		rstk=cbstk;
		stkp= &cbstkp;
		break;
	case CSTACK:
		rstk=ccstk;
		stkp= &ccstkp;
		break;
	}
	switch(type) {
	case PUSH:
		if (*stkp>=MAX_LEVELS)
			ER "TOO MANY NESTS" SX;
		rstk[*stkp]=currpush;
		(*stkp)+=1;
		break;
	case POP:
		(*stkp)-=1;
		break;
	default:
		ER "INTERNAL: Invalid stk operation" SX;
	}
}
		
static int 	ttab[7][7]=
	{
	/*YNUM     YINT     YSTR     YBTH     YSU      YNU      YUNK */

	{ YNUM,    YNUM,    YUNK,    YNUMUNK, YUNK,    YNUMUNK, YUNK },
	{ YNUM,    YINT,    YUNK,    YNUMUNK, YUNK,    YNUMUNK, YUNK },
	{ YUNK,    YUNK,    YSTR,    YSTRUNK, YSTRUNK, YUNK,    YUNK },
	{ YNUMUNK, YNUMUNK, YSTRUNK, YBTH,    YSTRUNK, YNUMUNK, YUNK },
	{ YUNK,    YUNK,    YSTRUNK, YSTRUNK, YSTRUNK, YUNK,    YUNK },
	{ YNUMUNK, YNUMUNK, YUNK,    YNUMUNK, YUNK,    YNUMUNK, YUNK },
	{ YUNK,    YUNK,    YUNK,    YUNK,    YUNK,    YUNK,    YUNK }
	};

static int	rttab[7][7]=
	{
		{ YNUM, (-1), YUNK, (-1), (-1), (-1), YUNK },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ YUNK, (-1), YSTR, (-1), (-1), (-1), YUNK },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ YUNK, (-1), YUNK, (-1), (-1), (-1), YUNK }
	};

resolve(tmp, from, to)
VARINFOP	tmp;
int	from;
int	to;
{
	int	fromval, toval;

	fromval=(tmp->rt)[from];
	toval=(tmp->rt)[to];
	(tmp->rt)[to]=rttab[fromval][toval];

	if (fromval==9 || toval==9)
		ER "Missed one" SX;
	switch ((tmp->rt)[to]) {
	case YUNK:
	case YSTR:
	case YNUM:
		break;
	default:
		fprintf(stderr, "(%d and %d) rt is now %d.\n", 
			fromval, toval, (tmp->rt)[to]);
		ER "Bad news in 'resolve'" SX;
		break;
	}	

	fromval=(tmp->cur)[from];
	toval=(tmp->cur)[to];
	(tmp->cur)[to]=ttab[fromval][toval];

	if ((tmp->rt)[to]<0 || (tmp->cur)[to]<0) {
		fprintf(stderr, "rt: %d.\n", (tmp->rt)[to]);
		fprintf(stderr, "(%d and %d, ttab=%d.\n", fromval, toval, ttab[fromval][toval]);
		ER "Bad news in 'resolve'" SX;
	}
}
