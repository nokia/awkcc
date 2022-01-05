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
	
resize(htb)
register HTBP	htb;
{
	register ELEMP	tmp, save;
	register int	i;
	unsigned int	j;
	ELEMP	*m_buckets();
	register HTBP	tmptab;
	
	SP(tmptab=(HTBP)malloc(sizeof(HTB)), "resize");
	tmptab->len=htb->len*GROWTAB;
	tmptab->next=tmptab->len*FULLTAB;
	tmptab->tab=m_buckets(tmptab->len);
	for (i=0; i<htb->len; i++)
		if (tmp=htb->tab[i])
			while (tmp) {
				save=tmp->next;
				j=tmp->hval%tmptab->len;
				tmp->next=tmptab->tab[j];
				tmptab->tab[j]=tmp;
				tmp=save;
			}
	free(htb->tab);
	htb->tab=tmptab->tab;
	htb->len=tmptab->len;
	htb->next=tmptab->next;
	free(tmptab);
}
