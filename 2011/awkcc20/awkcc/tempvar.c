/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <stdio.h>
#include "awkcc.h"

/*  Temporary variables:
 *
 *	1)	string temps.
 *	2)	var temps.
 *	3)	parameter temps.
 *	4)	function return temps.
 */

int	numstrtemps, numvartemps, numpartemps, numfuntemps, numarrtemps;
int	numifelsecommas;

/* The above will contain exportable values.  The below
 * are for internal use
 */

static int	numstrt1, numvart1, numpart1, numfunt1;

droprets(e, num, maybe)
Nodep	e;
int	num;
int	maybe;
{
	int	i;
	Nodep	tmp;
	char	*p;

	if (!e)
		return 0;
	/*  CONDEXPR self-evalords, but should be dealt with differently */
	if (SELF_EVABORDING(e->t) && e->t!=CONDEXPR)
		return;
	switch(e->t) {
	case USEDCALL:
	case CALL:
		if (hasret(e->s)) {
			p=(maybe?"C":"");
			PFO("%sFREEFTM(funtmp%d)\n", p, e->tempfun);
			num++;
		}
		break;
	case PRINT:
	case PRINTF:
	case GETLINE:
		num+=droprets(e->THREE, num, maybe);
	case SPRINTF:
	case ARRAY:
	case SYSARRAY:
	case ARRPARAM:
		for (tmp=e->LEFT; tmp; tmp=tmp->nnext)
			num+=droprets(tmp, num, maybe);
		break;
	case AND:
	case BOR:
	case CONDEXPR:
		num+=droprets(e->LEFT, num, maybe);
		maybe=1;
		for (i=1; i<(int) e->subs; i++) 
			num+=droprets(e->narg[i], num, maybe);
		break;
	default:
		for (i=0; i<(int) e->subs; i++) 
			num+=droprets(e->narg[i], num, maybe);
		break;
	}
}

tmpva(e)
Nodep	e;
{
	numpartemps=0;
	numstrtemps=0;
	numfuntemps=0;
	numvartemps=0;
	numarrtemps=0;
	numifelsecommas=0;
	tmpva1(e);
}

tmpva1(e)
Nodep	e;
{
	int	i;
	Nodep	tmp;

	/*  The idea is to call tmpva3 once with each individual
	 *  expression at its highest node, resetting the high
	 *  water mark counters first.
	 */
	if (!e) return;
	switch(e->t) {
	case FUNC:
		i=nextlen(e->LEFT);
		numpartemps=((i>numpartemps)?i:numpartemps);
		tmpva1(e->RIGHT);
		break;
	case PROGRAM:
		/*  Do for each PA and for each func, so that
		 *	no problems when called from emitfunc.
		 */
		for (i=0; i<(int) e->subs; i++) 
			for (tmp=e->narg[i]; tmp; tmp=tmp->nnext) {
				if (tmp->t==PASTAT2)
					tmp->tempif=numifelsecommas++;
				tmpva1(tmp);
			}
		break;
	case IF:
	case WHILE:
	case INWHILE:
	case FOR:
	case DO:
	case PASTAT2:
	case PASTAT:
		for (i=0; i<(int) e->subs; i++)
			tmpva1(e->narg[i]);
		break;
	case COMPSTMT:
		for (e=e->nnext; e; e=e->nnext)
			tmpva1(e);
		break;
	default:
		tmpva2(e);
	}
}
		
tmpva2(e)
Nodep	e;
{
	numstrt1=0;
	numvart1=0;
	numpart1=0;
	numfunt1=0;
	tmpva3(e);
	if (numstrt1>numstrtemps)
		numstrtemps=numstrt1;
	if (numvart1>numvartemps) 
		numvartemps=numvart1;
	if (numpart1>numpartemps)
		numpartemps=numpart1;
	if (numfunt1>numfuntemps)
		numfuntemps=numfunt1;
}

/*  This should be counting the number of temps needed in one
 *  full expression.
 */

tmpva3(e)
Nodep	e;
{
	int	i;
	Nodep	tmp;

	if (!e) return;
	switch(e->t) {
	case COMPSTMT:
	case IF:
	case INWHILE:
	case WHILE:
	case FOR:
	case DO:
	case PASTAT:
	case PASTAT2:
	case FUNC:
		tmpva1(e);
		break;
	case PRINTF:
	case SPRINTF:
		if ((i=nextlen(e->LEFT))>numpartemps)
			numpartemps=i-1;
	case CALL:
		/*  Par temps can be derived from funtemps!! */
		/*  Furthermore, funcalls should be self-converting! */
		e->tempfun=numfunt1++;
	default:
		if (!SELF_CONV(e->t) && e->w==YSTR && e->y!=YSTR)
			e->tempstr=numstrt1++;
		if (e->t==CAT 	||
		    /* next test for emitconcat */
		    (ISARRAY(e->t) && e->LEFT && e->LEFT->nnext) ||
		    e->t==SPRINTF  ||
		    e->t==SUBSTR ||
		    e->t==GSUB   ||
		    e->t==SUB ||
		    e->t==FTOUPPER ||
		    e->t==FTOLOWER ||
		    e->t==CONDEXPR ||
			 /* The following is currently implemented
			  * only for ++ and --, but may be
			  * necessary for assignment within
			  * expressions, also.  (Not really, I
			  * suppose, since the "order of side
			  * effects is unspecified"
			  */
		    e->flags&FIXORD) {
			e->tempvar=numvart1++;
		}
		/*  All the stuff that has an arglist goes here */
		switch(e->t) {
		case ARRAY:
		case SYSARRAY:
		case ARRPARAM:
		 	for (tmp=e->LEFT; tmp; tmp=tmp->nnext)
				tmpva3(tmp);
			break;
		case PRINT:
		case PRINTF:
			tmpva3(e->THREE);
		case SPRINTF:
		 	for (tmp=e->LEFT; tmp; tmp=tmp->nnext)
				tmpva3(tmp);
			break;
		case GETLINE:
			for (i=0; i<(int) e->subs; i++) {
				if (i==1) continue;
				tmpva3(e->narg[i]);
			}
			break;
		default:
			for (i=0; i<(int) e->subs; i++)
				tmpva3(e->narg[i]);
		}
	}
}
