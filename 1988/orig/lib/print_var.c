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
	
print_var(str, var)
char	*str;
VARP	var;
{
	fprintf(stderr, "%s(%d,", str, var);
	fprintf(stderr, "%d,", var->string);
	fprintf(stderr, "%d): ", S(var));
	fprintf(stderr, "N:%d, ", R(var));
	fprintf(stderr, "s:%s, ", S(var));
	fprintf(stderr, "n:%.6g, ", var->num);
	fprintf(stderr, "curr:%o, ", (char)var->cur);
	fprintf(stderr, "len:%d.\n", L(var));
}

print_htb(htb)
HTBP	htb;
{
	ELEMP	tmp;
	int	i;
	
	fprintf(stderr, "len: %d, num: %d, index: %d.\n",
			 htb->len, htb->num, htb->index);
	for (i=0; i<htb->len; i++) {
		fprintf(stderr, "Bucket %d.\n", i);
		if (tmp=htb->tab[i]) {
			for (; tmp; tmp=tmp->next) {
				fprintf(stderr, "Element %d.\n", i);
  				fprintf(stderr, "%d(%s), %s.\n", 
				        i, S(tmp), S(tmp->var));
			}
		}
	}
	fprintf(stderr, "Done with this table.\n");
}
