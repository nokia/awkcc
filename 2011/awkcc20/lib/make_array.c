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
	
HTBP
m_array()
{
	ELEMP	*m_buckets();
	register HTBP	tmp;
	
	SP(tmp=(HTBP)malloc(sizeof(HTB)), "m_array");
	tmp->num=0;
	tmp->len=STARTAB;
	tmp->next=tmp->len*FULLTAB;
	tmp->tab=m_buckets(tmp->len);
	tmp->lastelem=(ELEMP) NULL;
	return(tmp);
}

ELEMP	*
m_buckets(size)
register int	size;
{
	register ELEMP	*tmp2;
	register int	i;
	ELEMP	*tmp1;
	
	SP(tmp1=(ELEMP *) calloc (size, sizeof(ELEMP)), "m_buckets");
	tmp2=tmp1;
	for (i=0; i<size; i++)
		tmp2[i]=(ELEMP) NULL;
	return(tmp1);
}

free_array(htb)
HTBP	htb;
{
	register ELEMP	tmp, save;
	register int	i;
	
	for (i=0; i<htb->len; i++)
		if (tmp=htb->tab[i])
			while (tmp) {
				save=tmp->next;
				free_elem(tmp);
				tmp=save;
			}
	free(htb->tab);
	free(htb);
}
