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
	
extern VARP	Au_FILENAME;
extern VARP	tmpvar;
extern int	getfnr();

awkerr(s)
char	*s;
{
	fprintf(stderr, "\nawkcc: %s.\n", s);
	fprintf(stderr, "input record number %d, file %s.\n", getfnr(), TS(Au_FILENAME));
	EXIT(1, -1);
}
