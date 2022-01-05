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

#define MAX_SAVED 100

static int	avail;
static VARP	vars[MAX_SAVED];
extern STRP	nullstring;

VARP
m_evar()
{
	register VARP	a;

	if (avail)
		a=vars[avail--];
	else SP(a=(VARP)malloc(sizeof(VAR)), "m_evar");
	a->num=0.0;
	a->string=nullstring;
	nullstring->refcnt++;
	a->cur=INITNUM|INITSTR;
	return(a);
}

VARP
m_var()
{
	register VARP	a;

	if (avail)
		a=vars[avail--];
	else SP(a=(VARP)malloc(sizeof(VAR)), "m_var");
	a->num=0.0;
	a->string=m_str(1, "");
	a->cur=INITNUM|INITSTR;
	return(a);
}

free_var(a)
register VARP	a;
{
	free_str(a->string);
	if (avail+1<MAX_SAVED)
		vars[++avail]=a;
	else free(a);
}	
