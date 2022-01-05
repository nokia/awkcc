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

/* Basically bogus routine.  if 
 *
 *	for (i in j) delete i[j]
 *
 * then i should end up with some value.
 */

wclear_htb(htb, var)
HTBP	htb;
VARP	var;
{
	register ELEMP	tmp;
	register int	i;

	for (i=0; i<htb->len; i++) {
		if (htb->tab[i]) {
			tmp=htb->tab[i];
			R(tmp)++;
			free_str(var->string);
			var->string=tmp->string;
			clear_htb(htb);
			return;
		}
	}
}

clear_htb(htb)
HTBP	htb;
{
	register ELEMP	tmp;
	register int	i;
	register ELEMP	save;
	
	for (i=0; i<htb->len; i++) {
		if (htb->tab[i]) {
			tmp=htb->tab[i];
			while (tmp) {
				save=tmp->next;
				free_elem(tmp);
				tmp=save;
			}
			htb->tab[i]=(ELEMP) NULL;
		}
	}
	htb->num=0;
}
