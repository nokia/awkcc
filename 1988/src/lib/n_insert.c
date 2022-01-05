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

extern VARP	s_insert();
extern char	*numbers[];

VARP	
n_insert(n, htb)
long	n;
HTBP	htb;
{
	char	number[CONVSIZ];

	if (HAVENUM(n))
		return (s_insert(numbers[n], htb));
	else {
		sprintf(number, "%.6g", n);
		return(s_insert(number, htb));
	}
}
