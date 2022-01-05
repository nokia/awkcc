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
	
VARP	
member(a, htb)
char	*a;
HTBP	htb;
{
	register unsigned int	i;
	register unsigned int	j;
	register char	*s;
	register ELEMP	b;

	HASHFCN(a,s,i);	
	j=i;
	i%=htb->len;
	GETLAST(htb,j,a);
	if (b=htb->tab[i])
		for (; b; b=b->next)
			if (b->hval == j && !strcmp(S(b), a)) {
				SAVELAST(htb, b);
				return(b->var);
			}
	return((VARP) NULL);
}
