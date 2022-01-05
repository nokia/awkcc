/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <stdio.h>
#include "awk.h"
#include "y.tab.h"

uchar	*tostring(s)
register uchar	*s;
{
	register uchar	*p;

	p = (uchar *) Malloc(strlen(s)+1);
	if (p == NULL)
		awkerr("out of space in tostring on %s", s);
	strcpy(p, s);
	return(p);
}

Node *nodealloc(n)
{
	register Node *x;
	x = (Node *) Malloc(sizeof(Node) + (n-1)*sizeof(Node *));
	if (x == NULL)
		/* Used to be:  awkerror(FATAL */
		awkerr("out of space in nodealloc");
	x->nnext = NULL;
	/*
	x->lineno = lineno;
	*/
	return(x);
}

Node *node2(a,b,c) Node *b, *c;
{
	register Node *x;
	x = nodealloc(2);
	x->nobj = a;
	x->narg[0] = b;
	x->narg[1] = c;
	return(x);
}

Node *op2(a,b,c) Node *b, *c;
{
	register Node *x;
	x = node2(a,b,c);
	x->ntype = NEXPR;
	return(x);
}
