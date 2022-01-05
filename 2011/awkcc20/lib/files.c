/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <stdio.h>
#include "ear.h"
#include "hash.h"
	
extern char	errmsg[];

extern HTBP	INPIPES;
extern HTBP	OUTPIPES;
extern HTBP	INFILES;
extern HTBP	OUTFILES;
	
extern FILE	*files[MAXFILES];

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

filedrop(s)
char	*s;
{
	VARP	tmpvar, member();
	
	fflush(stdout);
	if (tmpvar=member(s, INFILES)) {
		fclose(files[(int)tmpvar->num]);
		files[(int)tmpvar->num]=(FILE *) NULL;
		delete(s, INFILES);
	}
	if (tmpvar=member(s, INPIPES)) {
		pclose(files[(int)tmpvar->num]);
		files[(int)tmpvar->num]=(FILE *) NULL;
		delete(s, INPIPES);
	}
	if (tmpvar=member(s, OUTPIPES)) {
		pclose(files[(int)tmpvar->num]);
		files[(int)tmpvar->num]=(FILE *) NULL;
		delete(s, OUTPIPES);
	}
	if (tmpvar=member(s, OUTFILES)) {
		fclose(files[(int)tmpvar->num]);
		files[(int)tmpvar->num]=(FILE *) NULL;
		delete(s, OUTFILES);
	}
	return 0;
}
