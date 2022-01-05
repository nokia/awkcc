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
