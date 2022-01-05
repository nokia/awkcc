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

static int	avail=0;
static ELEMP	elems[MAX_SAVED];
	
free_elem(e)
register ELEMP	e;
{
	if (avail+1<MAX_SAVED) {
		free_str(e->string);
		free_var(e->var);
		elems[++avail]=e;
	} else {
		free_str(e->var->string);
		free(e->var);
		free_str(e->string);
		free(e);
	}
}

ELEMP
m_elem(s)
char	*s;
{
	register ELEMP	tmp;
	VARP	m_var();
	VARP	m_evar();
	int	i;

	i=1+strlen(s);
	if (avail)
		tmp=elems[avail--];
	else SP(tmp=(ELEMP)malloc(sizeof(ELEM)), "m_elem"); 
	tmp->string=m_str(i, s);
	tmp->var=m_evar();
	tmp->next=(ELEMP) NULL;
	return(tmp);
}

ELEMP
s_m_elem(s)
STRP	s;
{
	register ELEMP	tmp;
	VARP	m_var();

	if (avail)
		tmp=elems[avail--];
	else SP(tmp=(ELEMP)malloc(sizeof(ELEM)), "s_m_elem");
	tmp->string=s;
	s->refcnt++;
	tmp->var=m_evar();
	tmp->next=(ELEMP) NULL;
	return(tmp);
}
