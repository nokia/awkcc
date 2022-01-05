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
extern HTBP	INFILES;
extern HTBP	OUTFILES;
	
extern FILE	*files[MAXFILES];

filedrop(s)
char	*s;
{
	VARP	tmpvar, member();

	if (tmpvar=member(s, INFILES)) {
		fclose(files[(int)tmpvar->num]);
		files[(int)tmpvar->num]=(FILE *) NULL;
		delete(s, INFILES);
	}
	if (tmpvar=member(s, OUTFILES)) {
		fclose(files[(int)tmpvar->num]);
		files[(int)tmpvar->num]=(FILE *) NULL;
		delete(s, OUTFILES);
	}
	if (tmpvar=member(s, INPIPES)) {
		pclose(files[(int)tmpvar->num]);
		files[(int)tmpvar->num]=(FILE *) NULL;
		delete(s, INPIPES);
	}
	if (tmpvar=member(s, OUTPIPES)) {
		/*fflush(stdout);*/
		pclose(files[(int)tmpvar->num]);
		files[(int)tmpvar->num]=(FILE *) NULL;
		delete(s, OUTPIPES);
	}
	return 0;
}
