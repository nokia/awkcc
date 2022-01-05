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
