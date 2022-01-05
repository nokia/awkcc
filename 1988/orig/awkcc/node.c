
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

extern int	lineno;

Nodep xmakenode(type, strval, numval)
int	type;
uchar	*strval;
double	numval;
{
	Nodep	retval;

	retval=makenode(type, -87, (TOKENP) NULL);
	retval->ntype=0;
	retval->s=tostring(strval);
	retval->n=numval;
	fix(retval);
	return(retval);
}

Nodep 
makenode(type, yields, tok)
int	type;
int	yields;
TOKENP	tok;
{
	Nodep 	n;
	int	i;

	if (!(n=(Nodep) calloc (1, sizeof(Node))))
		ER "calloc failed in makenode" SX;
	if (!(n->narg = (Nodep *) calloc (1, sizeof(Nodep))))
		ER "calloc failed in makenode" SX;
	n->narg[0]= (Nodep) NULL;
	n->called=0;
	n->t=type;
	n->rt=(-1);
	n->y=yields;
	n->w=YDEP;
	n->flags=0;
	n->subs=0;
	n->lineno=((tok!=(TOKENP) NULL)?tok->lineno:lineno);
	n->nnext=(Nodep) NULL;
	n->argnum=0;
	n->sympos=0;
	if (tok) {
		if (tok->type==VAR || tok->type==SYSVAR)
			n->n=(double)YUNK;
		else n->n=tok->num;
		i=strlen(tok->string);
		n->s=(uchar *) calloc (i+1, sizeof(uchar));
		strcpy(n->s, tok->string);
	} else {
		n->n=(double)YUNK;
		n->s=(uchar *)malloc(1);
		*(n->s)='\0';
	}
	return(n);
}

subsize(n, siz)
Nodep	n;
int	siz;
{
	int	save;
	int	i;

	save=n->subs;
	n->subs=siz;
	if (!(n->narg=(Nodep *) realloc (n->narg, siz*sizeof(Nodep))))
		ER "realloc failed in subsize" SX;
	for (i=save; i<siz; i++)
		n->narg[i]=(Nodep) NULL;
}

printnode(n, ind)
Nodep 	n;
int 	ind;
{
	rprintnode(n, ind, 1);
}

rprintnode(n, ind, first)
Nodep 	n;
int 	ind;
int	first;
{
	int	i;

	if (n == ((Nodep) NULL)) return;
	fprintf(stderr, "N%d(SP%d,L%d,S%d,W%d,Y%d,RT%d):%s,%g.\n", 
		n->t, 
		n->sympos,
		n->lineno, 
		n->subs, 
		n->w, 
		n->y, 
		n->rt, 
		n->s, 
		n->n);
	switch(n->t) {
	case PRINT:
	case PRINTF:
	case GETLINE:
		for (i=0; i<n->subs; i++) {
			if (i==1) continue;
			if (n->narg[i]) {
				indent(ind+1);
				fprintf(stderr, "%d,%d:", i, n->t);
				printnode(n->narg[i], ind+2);
			}
		}
		break;
	default:
		for (i=0; i<n->subs; i++) {
			if (n->narg[i]) {
				indent(ind+1);
				fprintf(stderr, "%d,%d:", i, n->t);
				printnode(n->narg[i], ind+2);
			}
		}
	}
	if (first) {
		for (n=n->nnext; n; n=n->nnext) { 
			indent(ind+1);
			fprintf(stderr, "-");
			rprintnode(n, ind+2, 0);
		}
	}
}

indent(ind)
int 	ind;
{
	while (ind--) fprintf(stderr, " ");
}

freenode(node)
Nodep node;
{
	int	i;

	if (node==(Nodep) NULL) return;
	for (i=0; i<node->subs; i++)
		if (node->narg[i])
			freenode(node->narg[i]);
	free(node->s);
	free(node->narg);
	free(node);
}

yyerror(s)
uchar	*s;
{
	extern uchar *cmdname, *curfname, *infilename;
	static int	been_here = 0;

	if (been_here++>2)
		return;
	fprintf(stderr, "%s %s: %s", cmdname, infilename, (s?s:(uchar*)""));
	fprintf(stderr, " at source line %d", lineno);
	if (curfname && *curfname)
		fprintf(stderr, " in function %s", curfname);
	fprintf(stderr, "\n");
	eprint();
	exit(-1);
}

ismem(str)
uchar	*str;
{
	VARINFOP	tmp;

	if ((tmp=memq(str, VAR, SYSVAR, 0)) ||
	    (tmp=memq(str, INTVAR, FLOATVAR, 0)) ||
	    (tmp=memq(str, ARRAY, SYSARRAY, 0)))
		return(tmp->type);
	return(0);
}
