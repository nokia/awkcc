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

extern double	atof();
extern char	*numbers[];

#define AREF1(v,n)	\
	if (R(v)) {		\
		R(v)--;		\
		v->string=m_str(n,"");	\
	} else {	\
		if (L(v)<n) {	\
			free(S(v));	\
			SP(S(v)=malloc(n), "refalloc");	\
			L(v)=n;	\
		} \
	}

#define AREF2(v,c,n)	\
	if (R(v)) {		\
		R(v)--;		\
		v->string=m_str(n,c);	\
	} else {	\
		if (L(v)<n) {	\
			free(S(v));	\
			SP(S(v)=malloc(n), "refalloc");	\
			L(v)=n;	\
		} \
		STRCPY2(n,S(v),c) \
	}

void
ToStr(a)
register VARP	a;
{
	register double	n;

#ifdef DEBUG
	if (STRAVAIL(a->cur)) {
		print_var("ToStr", a);
		ER "ToStr: Already converted" SX;
	}
#endif
	n=a->num;
	AREF1(a,CONVSIZ);
	if (HAVENUM(n)) {
		STRCPY(S(a), numbers[(int)n]);
	} else sprintf(S(a), "%.6g", n);
	a->cur|=NEWSTR;
}

anas(a,c)
register VARP	a;
char	*c;	/* string to assign */
{
	int	n;

	n=1+strlen(c);	
	AREF2(a,c,n);
	a->cur=INITSTR;
	a->cur|=isitanum(c);
	a->num=atof(c);
	a->cur|=NUMFLD;
}

/* 1 is decrement, 0 is increment */
char	*
postmp(decrp, a)
int	decrp;
register	VARP	a;
{
	register double	n;

	if (!NUMAVAIL(a->cur))
		TN(a);
	n=a->num;
	if (!GNUMSTR(a->cur)) {
		AREF1(a,CONVSIZ);
		if (HAVENUM(n)) {
			STRCPY(S(a), numbers[(int)n]);
		} else sprintf(S(a), "%.6g", n);
	}
	a->num+=(decrp?-1:1);
	a->cur=INITNUM;
	return(S(a));
}

char	*
premp(decrp, a)
int	decrp;
register VARP	a;
{
	register double	n;

	a->num+=(decrp?-1:1);
	n=a->num;
	AREF1(a,CONVSIZ);
	if (HAVENUM(n)) {
		STRCPY(S(a), numbers[(int)n]);
	} else sprintf(S(a), "%.6g", n);
	a->cur|=STRFLD;
	return(S(a));
}
	
VARP
vassign_str(a,c)
register VARP	a;
char	*c;	/* string to assign */
{
	int	n;

	n=1+strlen(c);
	AREF2(a,c,n);
	a->cur=INITSTR|MUSTFLD;
	return(a);
}

#include <varargs.h>
#include <ctype.h>

VARP
Atocase(a, s, d)
register VARP	a;
register char	*s;
int	d;
{
	register char	*dest, c;
	int	n;

	n=1+strlen(s);	
	AREF1(a,n);
	dest=S(a);
	while (c=(*s++))
		if (!d)
			*dest++=(isupper(c)?tolower(c):c);
		else *dest++=(islower(c)?toupper(c):c);
	*dest='\0';
	a->cur=INITSTR|MUSTFLD;
	return(a);
}

VARP
cat(a, nargs, va_alist)
register VARP	a;
int	nargs;
va_dcl
{
	int	n;
	register int	i;
	register char	*str;
	register char	*dest;
	va_list	ap;

	va_start(ap);
	for (i=0,n=1; i<nargs; i++) {
		n+=strlen(va_arg(ap, char *));
	}
	va_end(ap);
	AREF1(a,n);
	dest=S(a);
	va_start(ap);
	for (i=0; i<nargs; i++) {
		str=va_arg(ap, char *);
		while ((*dest++)=(*str++))
			;
		dest--;
	}
	a->cur=INITSTR|MUSTFLD;
	va_end(ap);
	return(a);
}
