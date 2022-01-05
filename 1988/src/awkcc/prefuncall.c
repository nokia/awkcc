
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

/* Currently, this is only configured for use with e->t==PRINT
 * as the initial call.
 */

prefuncall(e)
Nodep	e;
{
	register Nodep	tmp;
	register int	i;

	if (!e) return;
	switch(e->t) {
	case PRINT:
	case PRINTF:
		prefuncall(e->THREE);
	case SPRINTF:
	case ARRAY:
	case SYSARRAY:
	case ARRPARAM:
		for (tmp=e->LEFT; tmp; tmp=tmp->nnext);
			prefuncall(e->LEFT);
		break;
	case ADDEQ:
	case SUBEQ:
	case MULTEQ:
	case DIVEQ:
	case MODEQ:
	case POWEQ:
	case ASSIGN:
		prefuncall(e->LEFT);
		prefuncall(e->RIGHT);
		break;
	case GETLINE:
		prefuncall(e->LEFT);
		prefuncall(e->THREE);
		break;
	case CALL:
		for (tmp=e->LEFT; tmp; tmp=tmp->nnext)
			prefuncall(tmp);
		femit(e, ";\n");
		e->t=USEDCALL;
		break;
	default:
		for (i=0; i<e->subs; i++)
			prefuncall(e->narg[i]);
	}
}
