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

VARP	m_evar();

free_elem(e)
register ELEMP	e;
{
	free_str(e->string);
	free_var(e->var);
	free(e);
}

ELEMP
m_elem(s)
char	*s;
{
	register ELEMP	tmp;
	int	i;
	extern STRP	m_str();

	i=1+strlen(s);
	SP(tmp=(ELEMP)malloc(sizeof(ELEM)), "m_elem"); 
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

	SP(tmp=(ELEMP)malloc(sizeof(ELEM)), "s_m_elem");
	tmp->string=s;
	s->refcnt++;
	tmp->var=m_evar();
	tmp->next=(ELEMP) NULL;
	return(tmp);
}
