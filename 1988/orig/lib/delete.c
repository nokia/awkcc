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

ELEMP	
delete(a, htb)
char	*a;
HTBP	htb;
{
	register unsigned int	j;
	register unsigned int	i; 
	register char	*s;
	register ELEMP  b; 
	ELEMP	newhead;
	static	ELEM retval;

	/*  We return a pointer to something having a "next" pointing
	 *  to the next element which MAY BE NULL, or NULL if failed.
	 */
	HASHFCN(a,s,i);
	j=i;
	i%=htb->len;
	if (b=htb->tab[i]) {
		if (b->hval==j && !strcmp(S(b), a)) {
			newhead=htb->tab[i]->next;
			retval.next=newhead;
			free_elem(htb->tab[i]);
			htb->tab[i]=newhead;
			htb->num--;
			return((ELEMP) &retval);
		}
		for (; b->next; b=b->next)
			if (b->next->hval==j && !strcmp(S(b->next), a)) {
				newhead=b->next->next;
				free_elem(b->next);
				b->next=newhead;
				htb->num--;
				return(b);
			}
	}
	return((ELEMP) NULL);
}
