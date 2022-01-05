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
	
extern char	ctest[MAX_CHARS];
extern int	dflt;
	
clearctest(c)
char	c;
{
	register int	i;
	
	for (i=0; i<MAX_CHARS; i++)
		ctest[i]=0;
	/* newline is always a field separator
	 */
	ctest['\n']=1;
	ctest[c]=1;
	dflt=0;
}
