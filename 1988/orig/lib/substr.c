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
	
char	*
Asubstr(v, s, i, n, all)
register VARP	v;
char	*s;
int	i;
int	n;
int	all;
{
	int	j;

	j=strlen(s);
	if (i<=0) i=1;

	if (R(v)) {
		R(v)--;
		v->string=m_str(j-i+2, "");
	}
	*(S(v))='\0';
	if (j<i || (n<=0 && !all))
		return(S(v));
	if (all) {
		if (L(v)<j-i+2)
			SP(S(v)=realloc(S(v), j-i+2), "Asub");
		strcpy(S(v), ((i>0)?(s+i-1):s));
		return(S(v));
	}
	if (L(v) < ++n) 
		SP(S(v)=realloc(S(v), n), "Asub");
	strncpy(S(v), s+i-1, n-1);
	S(v)[n-1]='\0';
	return(S(v));
}
