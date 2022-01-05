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
#include "hash.h"

#define MAX_SAVED 100

static int	savail=0;
static STRP	strs[MAX_SAVED];
extern char	*numbers[];

STRP
m_str(n, s)
int	n;
char	*s;
{
	register STRP	tmp;

	if (savail) {
		tmp=strs[savail--];
		if (tmp->len<n) {
			free(tmp->str);	
			SP(tmp->str=malloc(n), "m_str1");
			tmp->len=n;
		}
	} else {
		SP(tmp=(STRP)malloc(sizeof(STR)), "m_str2");
		SP(tmp->str=malloc(n), "m_str3");
		tmp->len=n;
		tmp->refcnt=0;
	}
	STRCPY2(n,tmp->str,s);
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

	if (HAVENUM(d)) {
		STRCPY(numplace, numbers[(int) d])
		return m_str(10, numplace);
	} else {
		sprintf(numplace, "%.6g", d);
		return m_str(CONVSIZ, numplace);
	}
}

free_str(str)
register STRP	str;
{
	if (str->refcnt) {
		str->refcnt--;
	} else {
		if (savail+1<MAX_SAVED) {
			strs[++savail]=str;
		} else {
			free(str->str);
			free(str);
		}
	}
}
