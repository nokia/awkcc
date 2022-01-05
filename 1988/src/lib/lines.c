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

xgetl(v)
register VARP	v;
{
	extern int	NF;
	int	len;

	fflush(stdout);
	if (rgetrec(v)) {
		len=strlen(S(v))-1;
		if (S(v)[len]=='\n')
			S(v)[len]='\0';
		return 1;
	}
	return 0;
}

xfileget(f, v, type)
char	*f;
register VARP	v;
int	type;
{
	FILE	*pfp;
	int	len;
	extern FILE	*pipeget(), *fileget();
	extern int	upf[];
	extern VARP	*D;
	extern int	NF;

	switch(type) {
	case 1:
		pfp=fileget(f, "r");
		break;	
	case 2:
		pfp=pipeget(f, 0);
		break;
	default:
		awkerr("INTERNAL:  Illegal call to xfileget");
	}
	if (!pfp)
		return -1;
	if (v) {
		if (getrec(v, pfp)) {
			len=strlen(S(v))-1;
			if (S(v)[len]=='\n')
				S(v)[len]='\0';
			return 1;
		}
	} else {
		if (getrec(D[0], pfp)) {
			splitrec(S(D[0]), upf);
			assign_NF();
			return 1;
		}
	}	
	return 0;
}
