
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

extern uchar	*curfname;
extern int	infunc;
extern int	dbg;
extern int	verbose;

fix(e)
Nodep	e;
{
	Nodep	tmp;
	VARINFOP	tmpp;
	int	i;

	if (dbg) fprintf(stderr, "X%d\n", e->t);
	switch(e->t) {
	case SYM:
		if (infunc) add_glob(curfname, e);
		break;
	case ARG:
		tmpp=fnmemq(curfname, e->argnum);
		e->sympos=fnmemq(curfname, e->argnum)->sympos;
		break;
	case NEXT:
	case BREAK:
	case CONTINUE:
	case COMPSTMT:
	case PROGRAM:
	case CLINE:
	case EXTERN:
	case FUNC:
		break;
	case VARPARAM:
		/* This is never called in ambiguous cases */
		e->y=YUNK;
		tmpp=fnmemq(curfname, e->argnum);
		if (tmpp->type==ARRPARAM) ER "Illegal variable" SX;
		tmpp->type=VARPARAM;
		break;
	case ARRPARAM:
		e->y=YUNK;
		tmpp=fnmemq(curfname, e->argnum);
		if (tmpp->type==VARPARAM) 
			ER "Illegal array %s[]", e->s SX;
		tmpp->type=ARRPARAM;
		walkarray(e->LEFT);
		break;
	case ARRAY:
		e->y=YUNK;
		if (!strcmp(e->s, "ARGV"))
			e->t=SYSARRAY;
		walkarray(e->LEFT);
		add_array(e->s);
		break;
	case DELETE:
		e->y=YINT;
		setarrname(e->LEFT);
		subsize(e->LEFT, 1);
		e->LEFT->LEFT=e->RIGHT;
		e->RIGHT=(Nodep) NULL;
		e->subs=1;
		fix(e->LEFT);
		for (tmp=e->LEFT->LEFT; tmp; tmp=tmp->nnext) {
			setvarname(tmp);
			tmp->w=YSTR;
		}
		break;
	case VAR:
		e->y=YUNK;
		/* How does this fit into the "setvarname" business? */
		if (AMEMQ(e->s))
			ER "%s is an array", e->s SX;
		issysvar(e);
		add_var(e);
		break;
	case NUMBER:
		e->t=(((long)e->n==e->n)?INT:FLOAT);
		e->y=(e->t==INT)?YINT:YNUM;
		break;
	case IF:
		subf(e->LEFT, YBOOL);
		e->RIGHT=compstmt(e->RIGHT);
		e->THREE=compstmt(e->THREE);
		break;
	case PASTAT2:
		subf(e->LEFT, YBOOL);
		subf(e->RIGHT, YBOOL);
		e->THREE=compstmt(e->THREE);
		break;
	case DO:
		tmp=e->LEFT;
		e->LEFT=e->RIGHT;
		e->RIGHT=tmp;
	case INWHILE:
	case WHILE:
	case PASTAT:
		subf(e->LEFT, YBOOL);
		e->RIGHT=compstmt(e->RIGHT);
		break;
	case FOR:
		subf(e->RIGHT, YBOOL);
		e->FOUR=compstmt(e->FOUR);
		break;
	case IN:
		setvarname(e->LEFT);
		e->LEFT=op2(INTEST, e->LEFT, e->RIGHT);
		e->RIGHT=e->THREE;
		e->THREE=(Nodep) NULL;
		e->t=INWHILE;
		fix(e);
		break;
	case INTEST:
		e->y=YINT;
		for (tmp=e->LEFT; tmp; tmp=tmp->nnext)
			subf(tmp, YSTR);
		setarrname(e->RIGHT);
		break;
	case GSUB:
	case SUB:
		e->y=YINT;
		subf(e->LEFT, YSTR);
		subf(e->RIGHT, YSTR);
		subf(e->THREE, YSTR);
		if (e->LEFT->t==STRING || e->LEFT->t==REGEXPR)
			add_regexp(e->LEFT, 1);
		if (!VARIABLE(e->THREE->t) && !ASSIGNOP(e->THREE->t))
			ER "Can't gsub non-var yet" SX;
		break;
	case SPLIT:
		e->y=YINT;
		subf(e->LEFT, YSTR);
		subf(e->THREE, YSTR);
		setarrname(e->RIGHT);
		break;
	case PRINT:
		for (tmp=e->LEFT; tmp; tmp=tmp->nnext)
			subf(tmp, YDEP);
		subf(e->THREE, YSTR);
		break;
	case PRINTF:
	case SPRINTF:
		e->y=(e->t==PRINTF)?YINT:YSTR;
		for (tmp=e->LEFT; tmp; tmp=tmp->nnext)
			subf(tmp, YDEP);
		thearg(e->LEFT, 1)->w=YSTR;
		if (thearg(e->LEFT, 1)->t==STRING)
			decpf(e, &i);
		if (e->t==PRINTF)
			subf(e->THREE, YSTR);
		break;
	case CONDEXPR:
		e->y=YUNK;	/* necessary? */
		subf(e->LEFT, YBOOL);
		subf(e->RIGHT, YUNK);
		subf(e->THREE, YUNK);
		break;
	case GETLINE:
		e->y=YINT;
		subf(e->LEFT, YUNK);
		subf(e->THREE, YSTR);
		break;
	case INDEX:
		e->y=YINT;
		subf(e->RIGHT, YSTR);
		subf(e->LEFT, YSTR);
		break;
	case SUBSTR:
		e->y=YSTR;
		subf(e->THREE, YINT);
		subf(e->LEFT, YSTR);
		subf(e->RIGHT, YINT);
		break;
	case BOR:
	case AND:
		e->y=YINT;
		subf(e->LEFT, YBOOL);
		subf(e->RIGHT, YBOOL);
		break;
	case NOTMATCH:
	case MATCH:
		e->y=YINT;
		subf(e->LEFT, YSTR);
		subf(e->RIGHT, YSTR);
		if (e->RIGHT->t==STRING || e->RIGHT->t==REGEXPR)
			switch (regstr(e->RIGHT->s)) {
			case RREGEXP:
			case RREGEX:
				add_regexp(e->RIGHT, 0);
				break;
			}
		break;
	case EQ:
	case NE:
	case GT:
	case LT:
	case LE:
	case GE:
		e->y=YINT;
		setvarname(e->LEFT);
		setvarname(e->RIGHT);
		break;
	case CAT:
		e->y=YSTR;
		for (i=0; i<e->subs; i++)
			subf(e->narg[i], YSTR);
		break;
	case ADD:
	case MINUS:
	case MULT:
	case DIVIDE:
	case MOD:
		setvarname(e->LEFT);
		setvarname(e->RIGHT);
		arithbinop(e);
		break;
	case POWER:
		e->y=YNUM;
		subf(e->LEFT, YNUM);
		subf(e->RIGHT, YNUM);
		break;
	case NOT:
		e->y=YINT;
		subf(e->LEFT, YBOOL);
		break;
	case UMINUS:
		tmp=e->LEFT;
		e->y=tmp->w=(tmp->y==YINT?YINT:YNUM);
		setvarname(tmp);
		break;
	case PREDECR:
	case POSTDECR:
	case PREINCR:
	case POSTINCR:
	case FCOS:
	case FEXP:
	case FLOG:
	case FSIN:
	case FSQRT:
	case FATAN:
		e->y=YNUM;
		subf(e->LEFT, YNUM);
		break;
	case FINT:
	case FSRAND:
		e->y=YINT;
	case EXIT:
		subf(e->LEFT, YINT);
		break;
	case FLENGTH:
	case FSYSTEM:
	case CLOSE:
		e->y=YINT;
		subf(e->LEFT, YSTR);
		break;
	case INT:
		e->y=YINT;
		break;
	case FTOUPPER:
	case FTOLOWER:
		e->y=YSTR;
		subf(e->LEFT, YSTR);
		break;
	case FRAND:
	case FLOAT:
		e->y=YNUM;
		break;
	case STRING:
	case REGEXPR:
		e->y=YSTR;
		break;
	case RETURN:
		subf(e->LEFT, YUNK);
		break;
	case INDIRECT:
		e->y=YUNK;
		subf(e->LEFT, YINT);
		add_doll(e->LEFT);
		break;
	case MATCHFCN:
		e->y=YINT;
		subf(e->RIGHT, YSTR);
		if (e->RIGHT->t==STRING || e->RIGHT->t==REGEXPR)
			add_regexp(e->RIGHT, 1);
		subf(e->LEFT, YSTR);
		break;
	case CALL:
		e->y=YUNK;
		break;
	case ADDEQ:
	case SUBEQ:
	case MULTEQ:
	case DIVEQ:
	case MODEQ:
	case POWEQ:
	case ASSIGN:
		setvarname(e->LEFT);
		setvarname(e->RIGHT);
		tmp=e->LEFT;
		if (e->t==ASSIGN)
			e->y=tmp->w=tmp->y=e->RIGHT->w=e->RIGHT->y;
		else e->y=tmp->w=tmp->y=e->RIGHT->w=YNUM;
		break;
	default:
		fprintf(stderr, "Can't fix a %d.\n", e->t);
	}
}

Nodep thearg(parent, num)
register Nodep	parent;
int	num;
{
	register int	i;

	for (i=1; i<num && parent; i++)
		parent=parent->nnext;
	if (!parent)
		ER "INTERNAL: Arg %d does not exist", num SX;
	return(parent);
}

uchar	*tostring(s)
uchar	*s;
{
	int	i;
	uchar	*retval;

	if (!s) return(s);
	i=strlen(s);
	retval=(uchar *)malloc(i+1);
	strcpy(retval, s);
	return(retval);
}

#define YIELDFSINT(x)	(x->y==YINT)
#define YIELDSNUM(x)	(x->y==YNUM)
#define EXPLICIT(x)	(YIELDSNUM(x) || YIELDFSINT(x))

arithbinop(e)
Nodep	e;
{
	if (!(e->LEFT && e->RIGHT))
		ER "Illegal binary operator" SX;
	if (EXPLICIT(e->LEFT) && EXPLICIT(e->RIGHT)) {
		e->LEFT->w=e->LEFT->y;
		e->RIGHT->w=e->RIGHT->y;
		if (YIELDSNUM(e->LEFT) || YIELDSNUM(e->RIGHT))
			e->y=YNUM;
		else e->y=YINT;
	} else {
		if (EXPLICIT(e->RIGHT))
			e->RIGHT->w=e->RIGHT->y;
		else if (OPTVAR(e->RIGHT->t))
			e->RIGHT->w=YNUM;	/*YNUMERIC*/
		else e->RIGHT->w=YNUM;
		if (EXPLICIT(e->LEFT))
			e->LEFT->w=e->LEFT->y;
		else if (OPTVAR(e->LEFT->t))
			e->LEFT->w=YNUM;	/*YNUMERIC;*/
		else e->LEFT->w=YNUM;
		e->y=YNUM;
	}
}

Nodep	compstmt(e)
Nodep	e;
{
	Nodep	retval;

	if (e) {
		retval=op0(COMPSTMT);
		retval->nnext=e;
		retval->lineno=e->lineno;
		return(retval);
	}
	return (Nodep) NULL;
}

issysvar(e)
Nodep	e;
{
	if (e && e->t == VAR && wassysvar(e->s))
		e->t=SYSVAR;
}

walkarray(n)
Nodep	n;
{
	switch(nextlen(n)) {
	case 1:
		setvarname(n);
		if (OPTVAR(n->t))
			n->w=YSTRUNK;
		else if (CANDOUNK(n))
			n->y=n->w=YUNK;
		else n->w=YSTR;
		break;
	default:
		for (; n; n=n->nnext) {
			setvarname(n);
			n->w=YSTR;
		}
		break;
	}
}

subf(n, w)
Nodep	n;
int	w;
{
	if (n) {
		setvarname(n);
		n->w=w;
	}
}
