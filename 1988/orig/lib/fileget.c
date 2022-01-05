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
	
extern HTBP	INFILES;
extern HTBP	OUTFILES;
	
extern FILE	*files[MAXFILES];
extern char	errmsg[];

FILE	*
fileget(s, mode)
char	*s;
char	*mode;
{
	VARP	tmpvar, member(), s_insert();
	FILE	*tmpfp, *fopen();
	int	i;

	if (!*s)
		awkerr("null file name in print or getline");
	if (*s == '-' && *(s+1) == '\0') {
		if (*mode=='r')
			return(stdin);
		else return(stdout);
	}
	if (tmpvar=member(s, (*mode)=='r'?INFILES:OUTFILES)) 
		return(files[(int)tmpvar->num]);
	for (i=0; i<MAXFILES && files[i]; i++)
		;
	if (i>=MAXFILES) {
		sprintf(errmsg, "%s makes too many open files", s);
		awkerr(errmsg);
	}
	if (tmpfp=fopen(s, mode)) {
		tmpvar=s_insert(s, (*mode)=='r'?INFILES:OUTFILES);
		tmpvar->num=(double) i;
		/* This ms a certain amount of sense */
		if (*mode!='r' && isatty(tmpfp))
			setbuf(tmpfp, (char *) NULL);
		return (files[(int)tmpvar->num]=tmpfp);
	}
	delete(s, mode?INFILES:OUTFILES); 
	if (*mode!='r') {
		sprintf(errmsg, "Can't open %s file %s", 
			(*mode=='r'?"input":"output"), s);
		awkerr(errmsg);
	}
	return((FILE *) NULL);
}
