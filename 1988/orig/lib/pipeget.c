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
	
extern HTBP	INPIPES;
extern HTBP	OUTPIPES;
extern FILE	*files[MAXFILES];
extern char	errmsg[];

FILE	*
pipeget(s, mode)
char	*s;
int	mode;
{
	int	i;
	VARP	tmpvar, member(), s_insert();
	FILE	*popen();

	if (!*s)
		awkerr("null file name in print or getline");
	if (tmpvar=member(s, mode?OUTPIPES:INPIPES))
		return(files[(int)tmpvar->num]);
	for (i=0; i<MAXFILES && files[i]; i++)
		;
	if (i>=MAXFILES) {
		sprintf(errmsg, "%s ms too many open files", s);
		awkerr(errmsg);
	}
	tmpvar=s_insert(s, mode?OUTPIPES:INPIPES);
	tmpvar->num=(double) i;
	files[i]=popen(s, (mode?"w":"r"));
	if (!files[i]) {
		delete(s, mode?OUTPIPES:INPIPES);
		awkerr("Open of pipe failed.");	
	}
	return files[i];
}
