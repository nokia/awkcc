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
#include "hash.h"

extern char	*numbers[];

STRP
m_str(n, s)
int	n;
char	*s;
{
	register STRP	tmp;

	SP(tmp=(STRP)malloc(sizeof(STR)), "m_str2");
	SP(tmp->str=malloc(n), "m_str3");
	tmp->len=n;
	tmp->refcnt=0;
	strcpy(tmp->str,s);
#ifdef DEBUG
	if (tmp->refcnt)
		ER "Refcnt is %d for %s", tmp->refcnt, tmp->str SX;
#endif
	return(tmp);
}

STRP
m_strd(d)
register double	d;
{
	static char	numplace[CONVSIZ];
	extern char	*OFMT;

	if (HAVENUM(d)) {
		strcpy(numplace, numbers[(int) d]);
		return m_str(10, numplace);
	} else {
		sprintf(numplace, OFMT, d);
		return m_str(CONVSIZ, numplace);
	}
}

free_str(str)
register STRP	str;
{
	if (str->refcnt) {
		str->refcnt--;
	} else {
		free(str->str);
		free(str);
	}
}

/* change the size of the string */
char	*
sresize(v, n)
VARP	v;
int	n;
{
	n++;
	if (R(v)) {
		R(v)--;
		v->string=m_str(n, "");
	}
	if (L(v)<n) {
		L(v)=n;
		SP(S(v)=realloc(S(v), n), "sresize");
	}
	return(S(v));
}
