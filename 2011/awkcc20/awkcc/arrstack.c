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

static Nodep	*arrstk;
static int	allocated;
static int	arrlev;

arrpush(in) 
Nodep	in;
{
	if (!arrstk) {
		arrstk=(Nodep *) calloc (1, sizeof(Nodep));
		allocated=1;
		arrlev=0;
	}
	arrlev++;
	if (arrlev>allocated) {
		arrstk=(Nodep *)realloc(arrstk,arrlev*sizeof(Nodep));
		allocated++;
	}
	arrstk[arrlev-1]=in;
	return(arrlev);
}

arrpop() 
{
	arrstk[--arrlev]->flags|=FELEM;
}

chksub(e, a, which)
Nodep	e;
uchar	*a;
int	which;
{
	int	i, j;
	uchar	*v;

	v=e->s;
	for (i=arrlev; i; i--) {
		if (nextlen(e)==1 &&
		    VARNAME(e->t) &&
		    !(strcmp(v, arrstk[i-1]->LEFT->s)) &&
		    !(strcmp(a, arrstk[i-1]->RIGHT->s))) {
			j=arrstk[i-1]->flags&FELEM;
			if ((which && j) || !(which || j))
				return(i);
		}
	}
	return 0;
}

fixdel(e, a, pre)
Nodep	e;	/* subscript */
uchar	*a;	/* array string */
int	pre;
{
	int	i;
	uchar	*v;

	v=e->s;
	for (i=arrlev; i; i--) {
	    if (!strcmp(a, arrstk[arrlev-1]->narg[1]->s)) {
		if (nextlen(e)==1 &&
		    VARNAME(e->t) &&
		    !strcmp(v, arrstk[arrlev-1]->narg[0]->s) &&
		    !(arrstk[i-1]->flags&FELEM)) {
			if (pre)
				PFO("bkt%d= ", i);
			else {
				arrstk[i-1]->flags|=FELEM;
			}
	   	} else if (pre)
			fprintf(stderr, "WARNING: dangerous `delete %s[%s]' in walking loop.\n", a, v);
	   }
	}
}
