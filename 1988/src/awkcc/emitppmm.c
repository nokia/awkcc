
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
#include "awkcc.h"

extern int	changedollar;

#define ARGORDER	(e->flags&FIXORD 		&& 	\
	    		 e->LEFT->t != INTVAR 		&& 	\
	    		 e->LEFT->t != FLOATVAR)

emitppmm(e)
Nodep	e;
{
	/*  This only necessary when used as an arg, and there
	 *  are more than one increments/decrements of the same
	 *  variable.
	 */
	if (ARGORDER)
		PFO("S(cat(tmpvar%d, 1, ", e->tempvar);
	switch(e->LEFT->t) {
	case INTVAR:
	case FLOATVAR:
		switch(e->w) {
		case YSTR:
			PFO("%sTOA(tmpstr%d,", 
				(e->LEFT->t==INTVAR?"I":"F"),e->tempstr);
			break;
		case YNUM:
			PFO("((double) ");
			break;
		case YINT:
			PFO("((int) ");
			e->LEFT->w=YNUM;
			break;
		default:
			break;
		}
		PFO("X");
		tokname(2, e->t);
		e->LEFT->w=e->LEFT->y;
		femit(e->LEFT, ")");
		switch(e->w) {
		case YSTR: case YNUM: case YINT:
			PFO(")");
			break;
		}
		break;
	case INDIRECT:
		if (CHDOLL(e->LEFT->LEFT)) {
			changedollar=1;
			PFO("((pristine=0), ");
		}
	default:
		switch(e->w) {
		case YINT:
			PFO("((int) ");
			tokname(2, e->t);
			break;
		case YNUM:
			PFO("((double) ");
			tokname(2, e->t);
			break;
		case YSTR:
			tokname(3, e->t);
			break;
		default:
			tokname(2, e->t);
			break;
		}
		e->LEFT->w=YUNK;
		femit(e->LEFT, ")");
		if (e->w==YINT || e->w==YNUM) 
			PFO(")");
		if (e->LEFT->t==INDIRECT) {
			if (CHDOLL(e->LEFT->LEFT)) {
				changedollar=0;
				PFO(")");
			}
		}
		break;
	}
	if (ARGORDER)
		PFO("))");
}
