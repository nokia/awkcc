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

extern int	FNR;	
extern FILE	*in;

rgetrec(var)
register VARP	var;
{
	switch(getrec(var, in)) {
	case 0:
		if (opennext(1)) {
			resetfnr();
			FNR=0;
			return rgetrec(var);
		} else return 0;
		break;
	default:
		FNR++;
		return(1);
	}
	/* LINT
	 */
	return(0);
}
