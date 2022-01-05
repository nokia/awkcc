
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

extern int	dbg;
extern int	awkdbg;
extern uchar	*curfname;
extern VARINFOP	*symtab;
extern char	*npvar();

evalord(e)
Nodep		e;
{
	int	endit[ENDIT];
	int	i, j;
	int	fu;
	int	num;

	if (!e || SELF_EVABORDING(e->t))
		return;
	for (i=0; symtab[i]; i++) {
		if (!OPTVAR(symtab[i]->type)) continue;
		for (j=0; j<ENDIT; j++) endit[j]=0;
		num=0;
		fu=(-1);
		info_walk(e,&fu,&num,endit,symtab[i]->string);
		if (endit[YINT]||endit[YNUM]) {
			switch(fu) {
			case -1:
				ER "E error", symtab[i]->string);
				break;
			case YSTR:
				PFO("TN(Au_%s),",npvar(i));
				break;
			case YNUM:
			case YINT:
			case YNUMUNK:
				break;
			default:
				PFO("FN(Au_%s),",npvar(i));
				break;
			}
		}
		if (endit[YSTR]) {
			switch(fu) {
			case -1:
				ER "E error %s", symtab[i]->string SX;
				break;
			case YNUM:
			case YINT:
				PFO("ToStr(Au_%s),",npvar(i));
				break;
			case YBTH:
			case YSTRUNK:
			case YSTR:
				break;
			default:
				PFO("FS(Au_%s),",npvar(i));
				break;
			}
		}
		if (dbg)
			fprintf(stderr,
			"FORCING: %s (double:%d,int:%d,unk:%d,str,%d,fu:%d)\n",
			symtab[i]->string,
			endit[YNUM],endit[YINT],endit[YUNK],endit[YSTR],fu);
	}
}

#define INFORM(x) info_walk(x,fu,num,endit,name)

info_walk(e, fu, num, endit, name)
Nodep	e;
int	*fu;
int	*num;
int	endit[];
uchar	*name;
{
	int	i;
	Nodep	tmp, tmp1, tmp2;

	if (!e) return;
	switch(e->t) {
	case ARRAY:
	case SYSARRAY:
	case ARRPARAM:
	case SPRINTF:
		tmp2=tmp1=revlinks(e->LEFT);
		for (; tmp1; tmp1=tmp1->nnext)
			INFORM(tmp1);
		e->LEFT=revlinks(tmp2);
		break;
	case GETLINE:
		INFORM(e->LEFT);
		INFORM(e->THREE);
		break;
	case PRINT:
	case PRINTF:
		/*  None have more than three. */
		tmp2=tmp1=revlinks(e->LEFT);
		for (; tmp1; tmp1=tmp1->nnext)
			INFORM(tmp1);
		e->LEFT=revlinks(tmp2);
		INFORM(e->THREE);
		break;
	case ADDEQ:
	case SUBEQ:
	case MULTEQ:
	case DIVEQ:
	case MODEQ:
	case POWEQ:
	case ASSIGN:
		INFORM(e->LEFT);
	case FUNC:
		for (i=e->subs-1; i>=1; i--)
			INFORM(e->narg[i]);
		break;
	case PROGRAM:
		/*  This goes forward also, but it doesn't matter */
		for (i=0; i<e->subs; i++)
			for (tmp=e->narg[i]; tmp; tmp=tmp->nnext)
				INFORM(tmp);
		break;
	case COMPSTMT:
		/*  This goes "forward", but it doesn't matter */
		for (e=e->nnext; e; e=e->nnext)
			INFORM(e);
		break;
	default:
		for (i=e->subs-1; i>=0; i--)
			INFORM(e->narg[i]);
		break;
	}
	if (OPTVAR(e->t) && !strcmp(name, e->s)) {
		++*num;
		*fu=(int) e->n;
		endit[e->w]++;
	}
}

Nodep	revlinks(e)
Nodep	e;
{
	Nodep	tmp1;

	if (!e || !e->nnext) return(e);
	tmp1=e->nnext;
	e->nnext=(Nodep) NULL;
	return(linkum(revlinks(tmp1), e));
}
