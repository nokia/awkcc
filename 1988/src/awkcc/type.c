
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

static int	dontfollow;
extern int	OFSbug;
extern int	dbg;
extern int	currpush;
extern int	verbose;
extern uchar	*curfname;
extern int	infunc;
extern Nodep	winner;
extern int	lineno;
extern int	hasORS, hasOFS, hasSUBSEP, hasOFMT;
extern VARINFOP	*symtab;

pretype(symtab)
VARINFOP	*symtab;
{
	VARINFOP	tpar;
	Nodep	n;
	char	*p;
	int	endit[ENDIT];
	int	fu, num, again;
	int	isint, isflt, isunk, isstr, isdep;
	int	i, j;

	do {
		again=0;
		for (i=0; tpar=symtab[i]; i++) {
			if (!OPTVAR(tpar->type))
				continue;
			/* I only permit the following
			 * system variables to be reduced to	
			 * floats or ints.  Not argc, because
			 * assignment to it has side effects.
			 */
			if (tpar->type==SYSVAR &&
			    strcmp(tpar->string, "NF") 	&&
			    strcmp(tpar->string, "FNR")	&&
			    strcmp(tpar->string, "NR"))
				continue;
			for (j=0; j<ENDIT; j++) fu=num=endit[j]=0;
			for (j=0; j<tpar->ne; j++) {
				n=tpar->exprs[j];
				if (!strcmp(tpar->string, n->s)) {
					++num;
					fu=(int)n->n;
					endit[n->w]++;
				}
			}
			isstr=endit[YSTR];
			isunk=endit[YUNK]+endit[YSTRUNK]+endit[YBTH];
			isflt=endit[YNUM];
			isint=endit[YINT];
			isdep=endit[YDEP];
			if (UMEMQ(tpar->string))
				break;
			p="";
			if (!(isstr || isunk || isdep)) {
				tpar->type=isflt?FLOATVAR:INTVAR;
				p=isflt?"FLOATING-POINT":"INTEGER";
				foptvars(tpar);
				again=1;
			}
			if (verbose && *p)
				fprintf(stderr, "Variable %s -> %s.\n", 
						tpar->string, p);
			if (dbg || verbose) {
				fprintf(stderr,
				     "usage of |%s| - flt: %d, int: %d, unk: %d, str: %d.\n",
				     tpar->string, isflt, isint, isunk, isstr);
				fprintf(stderr, "\t");
				for (j=0; j<ENDIT; j++)
					fprintf(stderr, "%d ",  endit[j]);
				fprintf(stderr, "\n");
			}
		}
	} while (again);
}


/*  This routine adds statements at the beginning of the BEGIN()
/*  and USR() sections to install and update the values of various
/*  and sundry system variables.
 */

addsysvars(e)
Nodep	e;
{
	Nodep	tmp, tmp1;
	Nodep	makenode(), linkum();
	int	i;

	for (i=0; i<e->subs && ISFUNC(e->narg[i]); i++)
		;
	/*  publicize internal values */
	tmp=makenode(PASTAT, YDEP, (TOKENP) NULL);
	subsize(tmp, 2);
	tmp1=tmp->RIGHT=makenode(COMPSTMT, YDEP, (TOKENP) NULL);
	makeit(tmp1,	"ARGC",		"ARGC",		YINT,	EXTERN);
	makeit(tmp1,	"RSTART",	"0",		YSTR,	STRING);
	makeit(tmp1,	"RLENGTH",	"0",		YSTR,	STRING);
	makeit(tmp1,	"NF",		"NF",		YINT,	EXTERN);
	makeit(tmp1,	"NR",		"0",		YINT,	EXTERN);
	makeit(tmp1,	"FNR",		"0",		YINT,	EXTERN);
	hasORS=makeit(tmp1,	"ORS",	"\n",		YSTR,	STRING);
	makeit(tmp1,	"RS",		"\n",		YSTR,	STRING);
	hasSUBSEP=makeit(tmp1,	"SUBSEP","\034",	YSTR,	STRING);
	hasOFMT=makeit(tmp1,	"OFMT",	"%.6g",		YSTR,	STRING);
	makeit(tmp1,	"FS",		"S(Au_FS)",	YSTR,	EXTERN);
	hasOFS=makeit(tmp1,	"OFS",	" ",		YSTR,	STRING);
	if (tmp1 && nextlen(tmp1)>1)
		e->LEFT=linkum(tmp, e->LEFT);
	else {
		freenode(tmp);
		freenode(tmp1);
	}

	tmp=makenode(PASTAT, YDEP, (TOKENP) NULL);
	subsize(tmp, 2);
	tmp->RIGHT=makenode(COMPSTMT, YDEP, (TOKENP) NULL);
	tmp1=tmp->RIGHT;
	makeit(tmp1,	"NF",		"NF",		YINT,	EXTERN);
	makeinc(tmp1,	"NR");
	makeinc(tmp1,	"FNR");
	if (tmp1 && nextlen(tmp1)>1 && (e->RIGHT || e->THREE)) {
		e->RIGHT=linkum(tmp, e->RIGHT);
	} else {
		freenode(tmp);
		freenode(tmp1);
	}
}
		

Nodep	makeone(t, w, s)
int	t, w;
uchar	*s;
{
	Nodep	tmp;

	tmp=xmakenode(t, s, 0.0);
	tmp->w=w;
	tmp->y=w;
	return(tmp);
}

makeinc(node, s1)
Nodep	node;
uchar	*s1;
{
	Nodep	retval;
	
	if (!OVMEMQ(s1))
		return 0;
	retval=makenode(PREINCR, YINT, (TOKENP) NULL);
	retval->LEFT=makeone(VAR, YINT, s1);
	setvarname(retval->LEFT);
	node=linkum(node, retval);
	return 1;
}
	
makeit(node, s1, s2, type, rightis)
Nodep	node;
uchar	*s1;
uchar	*s2;
int	type;
int	rightis;
{
	Nodep	makenode();
	Nodep	retval;
	uchar	*malloc();

	if (!OVMEMQ(s1))
		return 0;
	retval=makenode(ASSIGN, type, (TOKENP) NULL);
	subsize(retval, 2);
	retval->LEFT=makeone(VAR, type, s1);
	retval->RIGHT=makeone(rightis, type, s2);
	setvarname(retval->LEFT);
	node=linkum(node, retval);
	return 1;
}

 
static int 	availtab[7][10]=
{
/* call:  availtab[current][wanted] */
/*
 YNUM    YINT    YSTR    YBTH    YSU     YNU  YUNK    YDEP YBO  */

{YNUM,   YNUM,   YBTH,   YNUMUNK,YBTH,   (-1),YNUM,   (-1),(-1)},
{YINT,   YINT,   YBTH,   YNUMUNK,YBTH,   (-1),YINT,   (-1),(-1)},
{YBTH,   YBTH,   YSTR,   YSTRUNK,YSTR,   (-1),YSTR,   (-1),(-1)},
{YBTH,   YBTH,   YBTH,   YBTH,   YBTH,   (-1),YBTH,   (-1),(-1)},
{YBTH,   YBTH,   YSTRUNK,YSTRUNK,YSTRUNK,(-1),YSTRUNK,(-1),(-1)},
{YNUMUNK,YNUMUNK,YBTH,   YNUMUNK,YBTH,   (-1),YNUMUNK,(-1),(-1)},
{YNUMUNK,YNUMUNK,YSTRUNK,YUNK,   YSTRUNK,(-1),YUNK,   (-1),(-1)}
};

static int	hasreturn;
/* FIX SUCH THAT GSUB AND SUB MAKE NOTE OF THEIR VARIABLE
 * CHANGING TYPES TO INITSTR.
 */

/* ALSO!  Do the same for getline */

follow(e)
Nodep		e;
{
	VARINFOP	tpar;
	Nodep	tmp;
	uchar	*savethefunc;
	int	i, j;
	int	y, z;

	if (!e) return;
	lineno=e->lineno;
	if (dbg) fprintf(stderr,"F: %d, %s, %.6g.\n",e->t,e->s,e->n);
	switch(e->t) {
	/* If we get a SYM or an ARG here, then either it
	   is an array (in the appropriate function) or we
	   can safely assume that it is a var.
	 */
	case SYM: case ARG:
		ER "INTERNAL:  obselete code" SX;
	case GETLINE:
		follow(e->THREE);
		if (!e->LEFT) {
			for (i=0; symtab[i]; i++) {
				if (symtab[i]->type==UVAR)
					numint(symtab[i]->string,YUNK,YUNK);
			}
			numint("FNR", YNUM, YINT);
			numint("NR", YNUM, YINT);
			numint("NF", YNUM, YINT);
		} else {
			tmp=e->LEFT;
			vnumint(tmp, YSTR, YSTR);
			if (NISREC(tmp))
				OFSbug=1;
		}
		break;
	case MATCHFCN:
		numint("RLENGTH", YNUM, YINT);
		numint("RSTART", YNUM, YINT);
		for (i=0; i<e->subs; i++)
			follow(e->narg[i]);
		break;
	case PRINT:
		follow(e->THREE);
		for (tmp=e->LEFT; tmp; tmp=tmp->nnext) {
			if (OPTVAR(tmp->t)) {
				tpar=symtab[tmp->sympos];
				tmp->n=(tpar->cur)[currpush];
				i=(int)tmp->n;
			} else {
				follow(tmp);
				i=tmp->y;
			}
			switch(i) {
			case YINT:
				tmp->w=hasOFMT?YNUM:YINT;
				break;
			case YNUM:
				tmp->w=YNUM;
				break;
			default:
				tmp->w=YSTR;
				break;
			}
		}
		break;
	case FUNC:
		infunc++;
		savethefunc=curfname;
		curfname=e->s;
		follow(e->RIGHT);
		curfname=savethefunc;
		infunc--;
		break;
	case CALL:
		if (dontfollow||fstkop(FIND,e->s)||!fstkop(CHECKOK)) {
			walkfunc(e);
			vstkop(NULLIFY);
		} else {
			fstkop(PUSH, e->s);
			genstkop(PUSH, CBFSTACK);
			walkfunc(e);
			follow(checkfunc(e->s)->RIGHT);
			genstkop(POP, CBFSTACK);
			fstkop(POP);
		}
		break;
	case PROGRAM:
		curfname=(uchar *)"";
		infunc=0;
		/*progfwalk(e->FOUR);*/
		fwalk(e->LEFT);
		vstkop(PUSH);
		vstkop(PUSH);
		for (i=0; symtab[i]; i++)
			if (symtab[i]->type==UVAR)
				numint(symtab[i]->string, YUNK, YUNK);
		fwalk(e->RIGHT);
		vstkop(POPR);
		fwalk(e->RIGHT);
		vstkop(POPR);
		fwalk(e->THREE);
		/*  Now, do analysis on the functions.  Nothing
		 *  is known upon entering a function, so zero
		 *  out global vars.  ALSO:  need to be
		 *  zeroed when doing recursive initial trace.
		 */
		progfwalk(e->FOUR);
		break;
	case NE:
	case EQ:
	case GT:
	case GE:
	case LT:
	case LE:
		comparison(e);
		follow(e->LEFT);
		follow(e->RIGHT);
		break;	
	case ASSIGN:
	case ADDEQ:
	case MULTEQ:
	case POWEQ:
	case SUBEQ:
	case DIVEQ:
	case MODEQ:
		follow(e->RIGHT);
		if (!OPTVAR(e->LEFT->t)) {
			e->flags|=PRECONV;
			if (NISREC(e->LEFT))
				OFSbug=1;
			follow(e->LEFT);
			break;
		}
		tpar=symtab[e->LEFT->sympos];
		switch((tpar->cur)[currpush]) {
			case YNUMUNK:
			case YINT:
			case YNUM:
			case YBTH:
				e->flags&=(~PRECONV);
				break;
			default:
				e->flags|=PRECONV;
				break;
		}
		switch(z=e->RIGHT->y) {
		case YINT:
			y=((e->LEFT->y==YINT)?YINT:YNUM);
			z=YNUM;
			goto FIN;
		case YNUM:
			y=z=YNUM;
			goto FIN;
		case YSTR:
			y=z=((e->t==ASSIGN)?YSTR:YNUM);
FIN:;
			e->y=e->LEFT->y=y;
			e->LEFT->w=e->RIGHT->w=y;
			break;
		case YUNK:
			if (e->t==ASSIGN) {
				if (OPTVAR((tmp=e->RIGHT)->t)) {
					y=tmp->n;
					z=tmp->rt;
				} else y=z=YUNK;
			} else {
				if (OPTVAR((tmp=e->RIGHT)->t)) {
					y=((tmp->n==YINT)?YINT:YNUM);
					z=YNUM;
				} else y=z=YNUM;
			}
			break;
		default:
			ER "INTERNAL: assignment of unknown type" SX;
		}
		vnumint(e->LEFT, z, y);
		e->LEFT->n=y;
		e->LEFT->rt=z;
		follow(e->LEFT);
		break;
	case SUB:
	case GSUB:
		for (i=0; i<e->subs; i++)
			follow(e->narg[i]);
		vnumint(e->THREE, YSTR, YSTR);
		if (NISREC(e->THREE))
			OFSbug=1;
		break;
	case FOR:
		follow(e->LEFT);
		genstkop(PUSH, CBSTACK);
		vstkop(PUSH);
		vstkop(PUSH);
		follow(e->RIGHT);
		genstkop(PUSH, CSTACK);
		vstkop(PUSH);
		follow(e->narg[3]);
		vstkop(POPR);
		genstkop(POP, CSTACK);
		follow(e->narg[2]);
		vstkop(POPR);
		follow(e->RIGHT);
		vstkop(PUSH);
		genstkop(PUSH, CSTACK);
		vstkop(PUSH);
		follow(e->narg[3]);
		vstkop(POPR);
		genstkop(POP, CSTACK);
		follow(e->narg[2]);
		vstkop(POP);
		vstkop(POPR);
		genstkop(POP, CBSTACK);
		break;
	case DO:
		genstkop(PUSH, CBSTACK);
		genstkop(PUSH, CSTACK);
		vstkop(PUSH);
		follow(e->RIGHT);
		vstkop(POPR);
		genstkop(POP, CSTACK);
		follow(e->LEFT);
		vstkop(PUSH);
		genstkop(PUSH, CSTACK);
		vstkop(PUSH);
		follow(e->RIGHT);
		vstkop(POPR);
		genstkop(POP, CSTACK);
		follow(e->LEFT);
		vstkop(POPR);
		genstkop(POP, CBSTACK);
		break;
	case WHILE:
	case INWHILE:
		genstkop(PUSH, CBSTACK);
		vstkop(PUSH);
		vstkop(PUSH);
		follow(e->LEFT);
		genstkop(PUSH, CSTACK);
		vstkop(PUSH);
		follow(e->RIGHT);
		vstkop(POPR);
		genstkop(POP, CSTACK);
		vstkop(POPR);
		follow(e->LEFT);
		vstkop(PUSH);
		genstkop(PUSH, CSTACK);
		vstkop(PUSH);
		follow(e->RIGHT);
		vstkop(POPR);
		genstkop(POP, CSTACK);
		vstkop(POP);
		vstkop(POPR);
		genstkop(POP, CBSTACK);
		break;
	case CONDEXPR:
	case IF:
		follow(e->LEFT);
		vstkop(PUSH);
		vstkop(PUSH);
		follow(e->RIGHT);
		vstkop(INVERT);
		if (e->THREE) {
			follow(e->THREE);
			vstkop(POPR);
			vstkop(INVERT);
			vstkop(POP);
		} else {
			vstkop(POPR);
			vstkop(POPR);
		}
		break;
	case PASTAT2:
		vstkop(PUSH);
		follow(e->LEFT);
		vstkop(POPR);
		vstkop(PUSH);
		follow(e->THREE);
		vstkop(POPR);
		vstkop(PUSH);
		follow(e->RIGHT);
		vstkop(POPR);
		break;
	case PASTAT:
		if (e->LEFT) {
			follow(e->LEFT);
			vstkop(PUSH);
			follow(e->RIGHT);
			vstkop(POPR);
		} else follow(e->RIGHT);
		break;
	case VAR:
	case SYSVAR:
		tpar=symtab[e->sympos];
		e->n=(tpar->cur)[currpush];
		e->rt=(tpar->rt)[currpush];
		switch(e->w) {
		case YUNK:
			switch((tpar->cur)[currpush]) {
			case YINT:
			case YSTR:
			case YNUM:
				e->y=((tpar->cur)[currpush]);
			}
		case YSTRUNK:
		case YBTH:
		case YINT:
		case YNUM:
		case YSTR:
			(tpar->cur)[currpush]=
				availtab[(tpar->cur)[currpush]][e->w];
			break;
		case YBOOL:
		case YDEP:
			break;
		default:
			fprintf(stderr, "Well, got a %d.\n", e->w);
			ER "INTERNAL" SX;
		}
		break;
	case POSTINCR:
	case POSTDECR:
	case PREDECR:
	case PREINCR:
		if (!OPTVAR(e->LEFT->t)) {
			if (NISREC(e->LEFT))
				OFSbug=1;
			follow(e->LEFT);
			break;
		}
		switch(e->w) {
		case YDEP:
		case YUNK:
			tpar=symtab[e->LEFT->sympos];
			j=(tpar->cur)[currpush];
			i=(j==YINT)?YINT:YNUM;
			e->y=e->LEFT->w=i;
			break;
		default:
			e->LEFT->w=e->w;
			break;
		}
		follow(e->LEFT);
		if ((e->t==PREDECR||e->t==PREINCR)&&e->w==YSTR)
			y=YBTH;
		else y=((e->y==YINT)?YINT:YNUM);
		vnumint(e->LEFT, YNUM, y);
		break;
	case AND:
	case BOR:
		follow(e->LEFT);
		vstkop(PUSH);
		follow(e->RIGHT);
		vstkop(POPR);
		break;
	case COMPSTMT:
		fwalk(e->nnext);
		break;
	case PRINTF:
		follow(e->THREE);
	case SPRINTF:
	case FATAN:
	case FEXP:
	case FLENGTH:
	case FTOUPPER:
	case FTOLOWER:
	case FSYSTEM:
	case FCOS:
	case FINT:
	case FLOG:
	case FRAND:
	case FSIN:
	case FSQRT:
	case FSRAND:
	case ARRAY:
		fwalk(e->LEFT);
		break;
	case RETURN:
		/*  Something similar needs to be done for NEXT */
		follow(e->LEFT);
		hasreturn=1;
		/*  zero or one? This has a bad effect if stuff
		 *  immediately follows a return, so this should
		 *  be checked for.
		 */
		vstkop(AFFECTF, 0);
		break;
	case NEXT:
		for (i=0; symtab[i]; i++)
			if (symtab[i]->type==UVAR)
				numint(symtab[i]->string, YUNK, YUNK);
		break;
	case CONTINUE:
	case BREAK:
		vstkop(AFFECT, (e->t==BREAK)?0:1);
		if (currpush<=0)
			ER "CURRPUSH ERROR:%d.\n", currpush SX;
		vstkop(POP);
		vstkop(PUSH);
		break;
	case ADD:
	case MINUS:
	case DIVIDE:
	case MOD:
	case MULT:
	default:
		for (i=0; i<e->subs; i++)
			follow(e->narg[i]);
	}
	if (dbg) fprintf(stderr, "FEXIT: %d, %s, %.6g.\n", e->t, e->s, e->n);
}

xparam(f, pos, n)
uchar	*f;
int	pos;
Nodep	n;
{
	register int	i;
	VARINFOP	tpar;

	if (!((tpar=fnmemq(f, pos)) && (*(tpar->string))))
		ER "too many arguments to function %s", f SX;
	switch(tpar->type) {
	case VARPARAM:
	case ARRPARAM:
		break;
	default: ER "INTERNAL:  illegal parameter type %d (%s)", tpar->type, tpar->string);
	}
}

Nodep	checkfunc(s)
Nodep	s;
{
	register Nodep	tmp;

	for (tmp=winner->narg[3]; tmp && strcmp(s, tmp->s); tmp=tmp->nnext)
		;
	if (tmp) return(tmp);
	ER "calling undefined function %s", s SX;
	/* NOTREACHED */
}

hasret(str)
uchar	*str;
{
	return(checkfunc(str)->flags & HASRET);
}

vnumint(n, rt, cur)
Nodep	n;
int	rt, cur;
{
	VARINFOP	tmp=(VARINFOP) NULL;
	extern VARINFOP	*symtab;

	if (OPTVAR(n->t)) {
		tmp=symtab[n->sympos];
		(tmp->rt)[currpush]=rt;
		(tmp->cur)[currpush]=cur;
	}
}

numint(vname, rt, cur)
char	*vname;
int	rt;
int	cur;
{
	VARINFOP	tmp;

	if (tmp=OVMEMQ(vname)) {
		(tmp->rt)[currpush]=rt;
		(tmp->cur)[currpush]=cur;
	}
}

/* Walk down from e to nexts */
fwalk(e)
Nodep	e;
{
	register Nodep	tmp;

	for (tmp=e; tmp; tmp=tmp->nnext)
		follow(tmp);
}

walkfunc(e)
Nodep	e;
{
	register Nodep	tmp;
	register int	i;

	for (i=1, tmp=e->LEFT; tmp; tmp=tmp->nnext, i++) {
		follow(tmp);
		xparam(e->s, i, tmp);
	}
}

progfwalk(e)
Nodep	e;
{
	Nodep	tmp;

	dontfollow=1;
	for (tmp=e; tmp; tmp=tmp->nnext) {
		vstkop(NULLIFY);
		hasreturn=0;
		genstkop(PUSH, CBFSTACK);
		follow(tmp);
		genstkop(POP, CBFSTACK);
		if (hasreturn)
			tmp->flags|=HASRET;
	}
	dontfollow=0;
}
