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

extern int	lineno, dbg, verbose;
extern VARINFOP	*symtab;
extern uchar	*curfname;

extern int	numfuntemps, numpartemps;

static int	tmppars=0;

#define OFFTHEEND(ve,fe)	(fe->called > ve->called)
#define PAR_DECL	0
#define PAR_PRNT	1
#define PAR_MAKE	2
#define PAR_FREE	3

emitfunc(e)
Nodep	e;
{
	int	hasretp;
	char	*p;
	VARINFOP	tmpp, funcpos;
	int	w;
	int	i;
	Nodep	tmp;
	uchar	*savethefunc;

	if (!e) return;
	if (dbg)
		fprintf(stderr, "FUNC: %d, %s, %.6g.\n", e->t, e->s, e->n);
	lineno=e->lineno;
	switch(e->t) {
	case RETURN:
		if (e->LEFT) {
			i=e->LEFT->y;
			evalord(e->LEFT);
			standard_string(i, &p, &w);
			PFO("A%s(retval,", p);
			e->LEFT->w=w;
			femit(e->LEFT, ");\n");
		}
		printp(curfname, PAR_FREE);
		droprets(e->LEFT, 0, FALSE);
		for (i=0; tmppars && i<numpartemps*numfuntemps; i++)
			PFO("FREESTR(partmp%d);\n", i);
		PFO("return retval;\n");
		break;
	case FUNC:
		tmpp=FMEMQ(e->s);
		if (verbose || dbg) {
			for (i=0; i<tmpp->ne; i++)
				fprintf(stderr, "WARNING: %s is global in %s.\n", tmpp->exprs[i]->s, e->s);
		}
		if (!FMEMQ(e->s)->called && verbose)
			fprintf(stderr, "WARNING: function %s never called\n", e->s);
		savethefunc=curfname;
		curfname=e->s;
		hasretp=e->flags & HASRET;
		PFO("%s\nAF_%s(", (hasretp?"VARP":"void"), e->s);
		printp(e->s,PAR_PRNT);
		PFO(")\n");
		printp(e->s,PAR_DECL);
		PFO("{\nVARP\tretval;\n");
		tmpva(e);
		if (numfuntemps>1 || numpartemps>1) {
			tmppars=1;
			for (i=0; i<numfuntemps; i++)
				PFO("VARP funtmp%d;\n", i);
			for (i=0; i<numpartemps*numfuntemps; i++)
				PFO("VAR partmp%d;\n", i);
			for (i=0; i<numpartemps*numfuntemps; i++)
				PFO("partmp%d.string=m_str(1,\"\");\n", i);
			for (i=0; i<numpartemps*numfuntemps; i++)
				PFO("partmp%d.num=0.0;\n", i);
			for (i=0; i<numfuntemps; i++)
				PFO("funtmp%d=(VARP) NULL;\n", i);
		} else tmppars=0;
		if (hasretp)
			PFO("retval=m_var();\n");
		printp(e->s,PAR_MAKE);
		emit(e->RIGHT);
		for (i=0; tmppars && i<numpartemps*numfuntemps; i++)
			PFO("FREESTR(partmp%d);\n", i);
		printp(e->s, PAR_FREE);
		if (hasretp)
			PFO("return retval;\n");
		PFO("}\n\n");
		curfname=savethefunc;
		emit(e->nnext);
		tmppars=0;
		break;
	case USEDCALL:
		/*  Only for print and sprintf, right now */
		if (hasret(e->s))
			PFO("funtmp%d", e->tempfun);
		else PFO("nullvar");
		break;
	case CALL:
		funcpos=FMEMQ(e->s);
		if (hasret(e->s))
			PFO("(funtmp%d=AF_%s(", e->tempfun, e->s);
		else PFO("(AF_%s(", e->s);
		tmpp=fnmemq(e->s, 1);
		for (i=1; tmpp; i++) {
			switch(tmpp->type) {
			case ARRPARAM:
				if (i>nextlen(e->LEFT)) {
					PFO("nullarray");
					if (OFFTHEEND(tmpp, funcpos))
						PFO(", 1");
					break;
				}
				tmp=thearg(e->LEFT, i);
				if (!ISARRAY(tmp->t)) 
					ER "INTERNAL: passing non-array" SX;
				pa(tmp);
				if (OFFTHEEND(tmpp, funcpos))
					PFO(", 0");
				break;
			case VARPARAM:
				if (i>nextlen(e->LEFT)) {
					PFO("(*nullvar)");
					break;
				}
				tmp=thearg(e->LEFT, i);
				makeunk(tmp, -1, i);
				break;
			default:
				ER "INTERNAL: impossible parameter type( %s, %d )", tmpp->string, tmpp->type SX;
			}
			if (tmpp=fnmemq(e->s, i+1))
				PFO(", ");
		}
		if (e->w==YDEP)
			p="))";
		else if (hasret(e->s))
			p="), funtmp%d)";
		else p="), nullvar)";
		PFO(p, e->tempfun);
		break;
	default:
		fprintf(stderr, "FUNCERR: %d\n", e->t);
		exit(-1);
	}
	if (dbg)
		fprintf(stderr, "FUNCEXIT: %d, %s, %.6g., w:%d, y:%d\n", e->t, e->s, e->n, e->w, e->y);
}

	
printp(fname, t)
char	*fname;
int	t;	/* 0 = type, 1 = printname, 2 = MAKE, 3 = FREE */
{
	VARINFOP	p;
	char	*s;
	int	i;

	for(i=1; p=fnmemq(fname, i); i++) {
		if (t==PAR_PRNT && verbose) {
			VARINFOP	tmp;
			tmp=FMEMQ(fname);
			if (OFFTHEEND(p,tmp)) {
				fprintf(stderr, "WARNING: parameter %s of %s",
					p->string, fname);
				if (p->called)
					fprintf(stderr, " is \"local\" in only %d of %d calls\n", tmp->called-p->called, tmp->called);
				else fprintf(stderr, " is always \"local\"\n");
			}
		}	
		switch(p->type) {
		case ARRPARAM:
			switch(t) {
			case PAR_DECL:	
				PFO("HTBP\tApa_%s;", p->string); 
				if (OFFTHEEND(p, FMEMQ(fname)))
					PFO("\nshort e%d;", i);
				break;
			case PAR_PRNT:	
				PFO("Apa_%s", p->string); 
				if (OFFTHEEND(p, FMEMQ(fname)))
					PFO(",e%d", i);
				if (p=fnmemq(fname, i+1))
					PFO(",");
				break;
			case PAR_MAKE:	
				if (OFFTHEEND(p, FMEMQ(fname))) {
					s="CMAKEARR(e%d, Apa_%s)\n";
					PFO(s, i, p->string);
				}
				break;
			case PAR_FREE:	
				if (OFFTHEEND(p, FMEMQ(fname)))
					PFO("CDROPARR(e%d,Apa_%s);\n",
				    	i,p->string);
				break;
			}
			break;
		case VARPARAM:
			switch(t) {
			case PAR_DECL:	
				PFO("VAR\tAp_%s;\n", p->string);
				break;
			case PAR_PRNT:	
				PFO("Ap_%s", p->string);
				if (p=fnmemq(fname, i+1))
					PFO(",");
				break;
			case PAR_MAKE:	
				PFO("COPYSTR(&Ap_%s);\n",p->string);
				break;
			case PAR_FREE:	
				PFO("FREESTR(Ap_%s);\n",p->string);
				break;
			}
			break;
		default:
			ER "INTERNAL: Unknown param type" SX;
		}
	}
}

makeunk(e, type, argno)
Nodep	e;
int	type, argno;	/* -1 for partmp, n:n>0 for tmpvar */
{
	char	*p;
	int	i, w;

	if (VARIABLE(e->t))
		i=YUNK;
	else i=e->y;
	switch(i) {
	case YSTR: p=(type>=0)?"NVASTR(":"VASTR("; w=YSTR; break;
	case YINT:
	case YNUM: p=(type>=0)?"NVANUM(":"VANUM("; w=YNUM; break;
	default: p=(type>=0)?"&":""; w=YUNK; break;
	}
	PFO(p);
	e->w=w;
	if (w==YUNK)
		memit("(*", e, ")");
	else {
		if (type>=0)
			PFO("tmpvar%d", type);
		else PFO("(&partmp%d)", argno-1+e->tempfun*numpartemps);
		memit(",", e, ")");
	}
}
