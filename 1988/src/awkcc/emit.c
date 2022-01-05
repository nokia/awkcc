
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

extern char	*epvar();
extern int	dbg;
extern VARINFOP	*symtab;
extern uchar	*curfname;
extern int	changedollar;
extern int	numpartemps;
extern int	lineno;
extern int	bufferflag;
extern int	awkdbg;
extern int	hasSUBSEP;

#define EWY(n,wt,yt)	(n->w==wt && n->y==yt)

pemit(c1, e)
char	*c1;
Nodep	e;
{
	memit(c1, e, "");
}

femit(e, c1)
char	*c1;
Nodep	e;
{
	memit("", e, c1);
}

memit(c1, e, c2)
char	*c1, *c2;
Nodep	e;
{
	PFO(c1);
	emit(e);
	PFO(c2);
}

emit(e)
Nodep	e;
{
	char	*p;
	int	i, x1, x2;
	Nodep	tmp;

	if (!e) return;
	if (awkdbg && e->ntype) pdbug(e->lineno);
	lineno=e->lineno;
	if (dbg) fprintf(stderr, "E: %d, %s, %.6g.\n", e->t, e->s, e->n);
	/*  type changing sometimes screws up assignments */
	if (ASSIGNOP(e->t)) {
		switch(e->y) {
		case YINT:
			if (e->LEFT->t!=INTVAR)
				e->y=YNUM;
			break;
		case YNUM:
			if (e->LEFT->t==INTVAR)
				e->y=YINT;
			break;
		}
	}
	if (e->w==YBOOL) {
		e->w=e->y;
		switch(e->y) {
		case YNUM: case YINT: break;
		case YSTR:
			memit("(*(", e, "))");
			return;
		case YUNK:
			memit("IF(", e, ")");
			return;
		default:
			printnode(e, 0);
			fprintf(stderr, "INTERNAL YBOOL ERROR(%d)\n",e->y);
		}
	}

	totype(e, 0);	
	switch(e->t) {
	case FUNC: case CALL: case USEDCALL: case RETURN: emitfunc(e);
		break;
	case PROGRAM: case PASTAT: case PASTAT2: case PRINT:
	case IF: case WHILE: case INWHILE: case DO: case FOR:
	case COMPSTMT: case CLOSE: case DELETE: case BREAK:
	case CONTINUE: case NEXT: case EXIT: case CLINE:
		emitstmt(e);
		break;
	case GETLINE:
		if (e->RIGHT) {
			p=e->LEFT?"L":"R";
			PFO("XGET%s(", p);
			emit(e->THREE);
			if (e->LEFT)
				memit(",", e->LEFT, "");
			p=(int)e->RIGHT=='|'?"2":"1";
			PFO(",%s)", p);
		} else {
			if (e->LEFT)
				memit("GETL(", e->LEFT, ")");
			else PFO("GETR");
		}
		break;
	case SPLIT:
		tokname(1, e->t);
		femit(e->LEFT, ",");
		pa(e->RIGHT);
		if (e->THREE)
			memit(",", e->THREE, ")");
		else PFO(",\"\")");
		break;
	case MATCHFCN:
		memit("MATCHFCN(", e->LEFT, ",");
		regout(e->RIGHT, 1, 0);
		PFO(")");
		break;
	case GSUB:
		PFO("G");
	case SUB:
		PFO("SUB(tmpvar%d,", e->tempvar);
		regout(e->LEFT, 1, 0);
		memit(",", e->RIGHT, ",");
		e->THREE->w=YUNK;
		if (ASSIGNOP(e->THREE->t)) {
			memit("(", e->THREE, ",");
			e->THREE->LEFT->w=YUNK;
			femit(e->THREE->LEFT, ")");
			PFO(NISREC(e->THREE->LEFT)?",1":",0");
		} else {
			emit(e->THREE);
			PFO(NISREC(e->THREE)?",1":",0");
		}
		PFO(")");
		break;
	case REGEXPR:
		printstring(e->s, 1, 0);
		break;
	case NOTMATCH:
		if (e->RIGHT->t==STRING || e->RIGHT->t==REGEXPR) {
			switch(regstr(e->RIGHT->s)) {
			case RREGEX: case RREGEXP: PFO("NO"); break;
			case RCARAT: case RNORMAL: PFO("!"); break;
			default:
				ER "INTERNAL: regexp type" SX;
				break;
			}
		} else PFO("NO");
	case MATCH:
		if (e->RIGHT->t==STRING || e->RIGHT->t==REGEXPR) {
			switch(regstr(e->RIGHT->s)) {
			case RREGEX:
			case RREGEXP:
				p="MATCH(";
				break;
			case RCARAT:
				p="HINDEX(";
				break;
			case RNORMAL:
				p="INDEX(";
				break;
			default:
				ER "INTERNAL: Wrong type for regexp" SX;
				break;
			}
		} else p="MATCH(";
		memit(p, e->LEFT, ",");
		regout(e->RIGHT, 0, 1);
		PFO(")");
		break;
	case FLENGTH:
		if (ISREC(e->LEFT)) {
			PFO("NC");
			break;
		}
	case FINT:
	case FCOS:
	case FEXP:
	case FLOG:
	case FSIN:
	case FSQRT:
	case FSYSTEM:
		tokname(1,e->t);
		femit(e->LEFT, ")");
		break;
	case FRAND:
		PFO("FRAND");
		break;
	case FSRAND:
		PFO("FSRAND(");
		if (ISREC(e->LEFT))
			PFO("(int)time((long *) 0))");
		else femit(e->LEFT, ")");
		break;
	case SUBSTR:
		PFO("SUBSTR(tmpvar%d,", e->tempvar);
		femit(e->LEFT, ",");
		femit(e->RIGHT, ",");
		if (e->THREE)
			femit(e->THREE, ",0)");
		else PFO("-1,1)");
		break;
	case INDEX:
		if (e->RIGHT->t==STRING && strlen(e->RIGHT->s)==1)
			PFO("C");
		memit("INDEX(", e->LEFT, ",");
		femit(e->RIGHT, ")");
		break;
	case SPRINTF:
		switch(e->LEFT->t) {
		case STRING:
			PFO("(sresize(tmpvar%d, 3096)", e->tempvar);
			/* unlimited; better than awk but has problems
			i=strlen(e->LEFT->s);
			PFO("(sresize(tmpvar%d, %d+", e->tempvar, i);
			for (tmp=e->LEFT->nnext; tmp; tmp=tmp->nnext) {
				if (tmp->w==YSTR) {
					if (tmp->t==STRING)
						PFO("%d", strlen(tmp->s));
					else if (tmp->y==YNUM || tmp->y==YINT)
						PFO("CONVSIZ");
					else {
						memit("strlen(", tmp, ")");
						if (tmp->t==CALL)
							tmp->t=USEDCALL;
					}
				} else PFO("CONVSIZ");
				PFO("+");
			}
			decpfnum(e->LEFT->s, &i);
			PFO("%d),sprintf(S(tmpvar%d)", i, e->tempvar);
			*/
			PFO(",sprintf(S(tmpvar%d)", e->tempvar);
			for (tmp=e->LEFT; tmp; tmp=tmp->nnext) {
				PFO(",");
				emit(tmp);
			}
			PFO("), S(tmpvar%d))", e->tempvar);
			break;
		default:
			PFO("xprintf(1,tmpvar%d", e->tempvar);
			tailxprint(e);
			break;
		}
		break;
	case PRINTF:
		switch(e->LEFT->t) {
		case STRING:
			efile1(e->THREE, "fprintf(", (int)e->RIGHT);
			for (tmp=e->LEFT; tmp; tmp=tmp->nnext)
				pemit(",", tmp);
			PFO(")");
			break;
		default:
			efile1(e->THREE, "xprintf(0, ", (int)e->RIGHT);
			tailxprint(e);
			break;
		}
		if (bufferflag && e->THREE)
			PFO(",fflush(pfp)");
		break;	
	case CONDEXPR:
		PFO("((");
		evalord(e->LEFT);
		femit(e->narg[0], ")?(");
		evalord(e->narg[1]);
		makeunk(e->narg[1], e->tempvar);
		PFO("):(");
		evalord(e->narg[2]);
		makeunk(e->narg[2], e->tempvar);
		PFO("))");
		break;
	case AND:
	case BOR:
		memit("((", e->LEFT, ")");
		tokname(1,e->t);
		memit("(", e->RIGHT, "))");
		break;
	case PREINCR:
	case PREDECR:
	case POSTINCR:
	case POSTDECR:
		emitppmm(e);
		break;
	case EXTERN:
	case FLOATVAR:
	case INTVAR:
		/* In this case, tokname==print out prefix */
		tokname(1,e->t);
		PFO("%s", e->s);
		break;
	case INDIRECT:
		checklf(e);
		switch (e->w) {
		case YINT:
			PFO("((int)");
		case YNUM: p="NUM"; break;
		case YSTR: p="STR"; break;
		case YDEP:	/* $1 byitself */
		case YUNK: p="VARS"; break;
		default: ER "Illegal statement" SX;
		}
		if (VARIABLE(e->LEFT->t)) {
			e->LEFT->w=YUNK;
			PFO("V");
		}
		PFO("GETD%s(%d,", p, changedollar);
		changedollar=0;
		femit(e->LEFT, e->w==YINT?"))":")");
		break;
	case ASSIGN:
		if (OPTUNK(e->RIGHT))
			e->RIGHT->y=e->RIGHT->w=YUNK;
		switch(e->RIGHT->y) {
		case YUNK: p="VARS"; break;
		case YSTR: p="STR"; break;
		case YINT:
		case YNUM: p="NUM"; break;
		default: ER "INTERNAL ASSIGN" SX;
		}
		switch(e->LEFT->t) {
		case INDIRECT:
			checklf(e);
			if (VARIABLE(e->LEFT->LEFT->t)) {
				e->LEFT->LEFT->w=YUNK;
				PFO("V");
			}
			e->RIGHT->w=e->RIGHT->y;
			switch(e->RIGHT->y) {
			case YINT:
				e->RIGHT->w=YNUM;
				break;
			}
			PFO("AD%s(", p);
			emit(e->LEFT->LEFT);
			memit(",", e->RIGHT, ")");
			break;
		case SYSVAR:
			/*  These are the only non-readonly
			 *  system variables thus far.
			 */	
			if (SISL(e, "FS")   || SISL(e, "ORS") ||
			    SISL(e, "OFMT") || SISL(e, "SUBSEP") ||
			    SISL(e, "RS")   || SISL(e, "OFS")) {
				e->LEFT->w=YUNK;
				e->RIGHT->w=e->RIGHT->y;
				PFO("A%s%s(", e->LEFT->s, p);
				femit(e->LEFT, ",");
				if (e->RIGHT->y==YINT && e->LEFT->t != INTVAR)
					e->RIGHT->w=YNUM;
				femit(e->RIGHT, ")");
				break;
			}
			/*  otherwise, fall through */
		case VAR:
		case ARRPARAM:
		case VARPARAM:
		case PARAMETER:
		case ARRAY:
		case SYSARRAY:
			e->LEFT->w=YUNK;
			e->RIGHT->w=e->RIGHT->y;
			PFO("A%s(", p);
			femit(e->LEFT, ",");
			if (e->RIGHT->y==YINT && e->LEFT->t != INTVAR)
				e->RIGHT->w=YNUM;
			femit(e->RIGHT, ")");
			break;
		case INTVAR:
		case FLOATVAR:
			e->LEFT->w=e->LEFT->y;
			memit("(", e->LEFT, "=");
			femit(e->RIGHT, ")");
			break;
		}
		break;
	case ADDEQ:
	case SUBEQ:
	case MODEQ:
	case DIVEQ:
	case MULTEQ:
	case POWEQ:
		if (OPTUNK(e->RIGHT))
			e->RIGHT->w=YUNK;
		switch(e->LEFT->t) {
		case INDIRECT:
			if (CHDOLL(e->LEFT->LEFT)) {
				changedollar=1;
				PFO("((pristine=0), ");
			}
		case ARRAY: case VAR: case SYSVAR: case SYSARRAY:
		case ARRPARAM: case VARPARAM: case PARAMETER:
			if (e->flags&PRECONV)
				PFO("C");
			tokname(2, e->t);
			e->LEFT->w=YUNK;
			emit(e->LEFT);
			memit(",", e->RIGHT, ")");
			if (e->LEFT->t==INDIRECT) {
				if (CHDOLL(e->LEFT->LEFT)) {
					changedollar=0;
					PFO(")");
				}
			}
			break;
		case INTVAR:
			if (e->t==MODEQ) {
				PFO("(");
				emit(e->LEFT);
				tokname(1,e->t);
				e->RIGHT->w=YINT;
				femit(e->RIGHT, ")");
				break;
			}
		case FLOATVAR:
			PFO("(");
			if (e->t==POWEQ) {
				e->LEFT->w=YUNK;
				femit(e->LEFT, "=pow(");
				e->LEFT->w=YNUM;
				emit(e->LEFT);
				e->RIGHT->w=YNUM;
				memit(",", e->RIGHT, ")");
			} else if (e->t==MODEQ) {
				e->LEFT->w=e->LEFT->y;
				emit(e->LEFT);
				PFO("=FLTMOD(Af_%s, ", e->LEFT->s);
				e->RIGHT->w=YNUM;
				femit(e->RIGHT, ")");
			} else {
				e->LEFT->w=e->LEFT->y;
				emit(e->LEFT);
				tokname(1,e->t);
				emit(e->RIGHT);
			}
			PFO(")");
			break;
		}
		break;
	case CAT:
		if (FAKECAT(e)) {
			i=FAKELCAT(e, 0)?1:0;
			e->narg[i]->w=YSTR;
			emit(e->narg[i]);
		} else {
			p=e->w==YUNK?"":"S";
			i=catleaves(e);
			PFO("%s(cat(tmpvar%d,%d,",p,e->tempvar,i);
			emitcatleaves(e, i);
			PFO("))");
		}
		break;
	case FTOUPPER:
	case FTOLOWER:
		p=e->w==YUNK?"":"S";
		PFO("%s(Atocase(tmpvar%d,", p, e->tempvar);
		femit(e->LEFT, e->t==FTOUPPER?",1))":",0))");
		break;
	case UMINUS:
	case NOT:
		PFO("(");
		tokname(1,e->t);
		femit(e->LEFT, ")");
		break;
	case ADD:
	case MINUS:
	case DIVIDE:
	case MULT:
	case MOD:
	case POWER:
		if (e->t==MOD) {
			int	ans;

			ans=(e->LEFT->y==YINT && e->RIGHT->y==YINT)?1:0;
			p=ans?"INT":"FLT";
			e->LEFT->w=e->RIGHT->w=(ans?YINT:YNUM);
			PFO("%sMOD",p);
		} else if (e->t==POWER) {
			e->LEFT->w=e->RIGHT->w=YNUM;
			PFO("pow");
		}
		memit("(", e->LEFT, "");
		tokname(1,e->t);
		femit(e->RIGHT, ")");
		break;
	case GT:
	case GE:
	case LT:
	case LE:
	case EQ:
	case NE:
		PFO("(");
		x1=(e->LEFT->w==YUNK || e->LEFT->w==YBTH)?1:0;
		x2=(e->RIGHT->w==YUNK || e->RIGHT->w==YBTH)?1:0;
		if (x1+x2==1) {
			Nodep	tmpl, tmpr;
			tmpl=x1?e->LEFT:e->RIGHT;
			tmpr=x1?e->RIGHT:e->LEFT;
			p=x1?"NO":"";
			switch(tmpr->w) {
			case YNUM:
				memit("docompN(", tmpl, ",");
				femit(tmpr, ",");
				tokname(0, e->t);
				PFO(",%sREV)", p);
				break;
			default:
				printnode(e, 0);
				ER "tmpr->w==%d.\n", tmpr->w SX;
			}
		} else switch(e->LEFT->w) {
			case YNUM:
			case YINT:
				if (e->RIGHT->w!=e->LEFT->w) 
					e->LEFT->w=e->RIGHT->w=YNUM;
				emit(e->LEFT);
				tokname(1,e->t);
				emit(e->RIGHT);
				break;
			case YSTR:
				memit("strcmp(", e->LEFT, ",");
				femit(e->RIGHT, ")");
				tokname(1,e->t);
				PFO("0");
				break;
			case YBTH:
			case YUNK:
				memit("docomp(", e->LEFT, ",");
				femit(e->RIGHT, ",");
				tokname(0, e->t);
				PFO(")");
				break;
			default:
				printnode(e, 0);
				ER "e->LEFT->w==%d.\n",e->LEFT->w SX;
		}
		PFO(")");
		break;
	case STRING:
		switch(e->w) {
		case YSTR:
			printstring(e->s, 1, 0);
			break;
		case YNUM:
			PFO("%f", atof(e->s));
			break;
		case YDEP:
		case YINT:
			PFO("%d", atoi(e->s));
			break;
		}
		break;
	case FLOAT:
		switch(e->w) {
		case YSTR:
			PFO("\"%g\"", e->n);
			break;
		case YDEP:
		case YNUM:
			PFO("%20.20g", e->n);
			break;
		case YINT:
			PFO("%d", (int) e->n);
			break;
		}
		break;
	case INT:
		switch(e->w) {
		case YSTR: p="\"%d\""; break;
		case YNUM: p="%d.0"; break;
		case YDEP: case YINT: p="%d"; break;
		default: ER "INTERNAL, (int)" SX;
		}
		PFO(p, (int)e->n);
		break;
	case ARRPARAM:
	case ARRAY:
	case SYSARRAY:
		switch (e->w) {
		case YSTR:
			PFO("TS(");
			break;
		case YINT:
			PFO("((int)");
		case YNUM:
			PFO("TONUM(");
			break;
		}
		if (i=chksub(e->LEFT, e->s, 0))
			PFO("elem%d->var", i);
		else {
			tmp=e->LEFT;
			if (tmp->w==YUNK || tmp->w==YSTRUNK)
				PFO("insert(0,");
			else if (nextlen(tmp)>1) {
				PFO("insert(0,");
				tmp->w=YUNK;
			} else if (tmp->w==YNUM)
				PFO("n_insert(");
			else if (tmp->t==POSTDECR || tmp->t==POSTINCR) {
				tokname(4,tmp->t);
				e->LEFT=e->LEFT->LEFT;
				e->LEFT->w=YUNK;
			} else PFO("s_insert(");
			emitconcat(e->LEFT);
			PFO(",");
			pa(e);
			PFO(")");
		}
		if (e->w==YINT||e->w==YNUM||e->w==YSTR)
			PFO(")");
		if (e->w==YINT)
			PFO(")");
		break;
	case INTEST:
		tokname(0,e->t);
		emitconcat(e->LEFT);
		PFO(",");
		pa(e->RIGHT);
		PFO(")");
		break;
	case VARPARAM:
		switch (e->w) {
		case YSTR:
			PFO("TS(");
			break;
		case YINT:
			PFO("((int)");
		case YNUM:
			PFO("TONUM(");
			break;
		}
		PFO("(&Ap_%s)", e->s);
		if (e->w!=YUNK)
			PFO(")");
		if (e->w==YINT)
			PFO(")");
		break;
	case VAR:
	case SYSVAR:
		switch (e->w) {
		case YSTR:
			PFO("S(Au_%s)", epvar(e));
			break;
		case YINT:
			PFO("((int)");
		case YNUM:
			PFO("Au_%s->num", epvar(e));
			if (e->w==YINT) 
				PFO(")");
			break;
		case YSTRUNK:
		case YUNK:
		case YBTH:
			PFO("Au_%s", epvar(e));
			break;
		}
		break;
	default:
		ER "EERR: %d.\n", e->t SX;
	}
	totype(e, 1);
	if (dbg) fprintf(stderr, "EXIT: %d, %s, %.6g., w:%d, y:%d\n", e->t, e->s, e->n, e->w, e->y);
}

efile1(e, func, v)
Nodep	e;
char	*func;
int	v;
{
	PFO("%s", func);
	if (bufferflag)
		PFO("pfp=");
	inefile(e, v);
}

efile2(e, v)
Nodep	e;
int	v;
{
	PFO("pfp=");
	inefile(e, v);
	PFO(";\n");
}


inefile(e, v)
Nodep	e;
int	v;
{
	switch(v) {
	case GT:
		memit("fileget(", e, ", \"w\")");
		break;
	case APPEND:
		memit("fileget(", e, ", \"a\")");
		break;
	case '|':
		memit("pipeget(", e, ",1)");
		break;
	case 0:
		PFO("stdout");
		break;
	}
}

emitconcat(e)
Nodep	e;
{
	if (e->nnext) {
		if (e->w==YUNK) {
			e->w=YSTR;
		} else PFO("S");
		PFO("(cat(tmpvar%d,%d,", e->tempvar,2*nextlen(e)-1);
		for (; e; e=e->nnext) {
			emit(e);
			if (e->nnext)
				PFO(hasSUBSEP?",SS,":",\"\\034\",");
		} 
		PFO("))");
	} else emit(e);
}

nextlen(e)
Nodep	e;
{
	register int 	i;
	
	for (i=0; e; i++, e=e->nnext)
		;
	return(i);
}

regout(v, t, fancy)
Nodep	v;
int	t;
int	fancy;
{
	int	i;

	if (v->t==STRING || v->t==REGEXPR) {
		if (fancy) {
			switch(regstr(v->s)) {
			case RREGEX:
			case RREGEXP:
				i=v->sympos;
				PFO("reg%s%d", (t?"1_":""), i);
				break;
			case RCARAT:
				printstring(v->s, 0, 1);
				break;
			case RNORMAL:
				printstring(v->s, 0, 0);
				break;
			default:
				ER "INTERNAL:, Wrong type for regexp" SX;
				break;
			}
		} else {
			i=v->sympos;
			PFO("reg%s%d", (t?"1_":""), i);
		}
	} else {
		memit("ins_regexp(", v, ", ");
		PFO("%d)", t);
	}
}

/* Check for legal field */
checklf(v)
Nodep	v;
{
	if (v->LEFT->t==STRING && (*(v->LEFT->s)<'0' || *(v->LEFT->s)>'9'))
		ER "Field $(%.6g) is illegal.\n", (int)atof(v->LEFT->s) SX;
}

tailxprint(e)
Nodep	e;
{
	register Nodep	tmp;
	register int	i;
	int	w;
	char	*p;

	PFO(", %d, ", nextlen(e->LEFT)-1);
	/*  This always exists, and is the control */
	emit(e->LEFT);
	i=0;
	for (tmp=e->LEFT->nnext; tmp; tmp=tmp->nnext, i++) {
		switch((VARIABLE(tmp->t))?YUNK:tmp->y) {
		case YINT:
		case YNUM:
			p="NVANUM(";
			w=YNUM;
			break;
		case YSTR:
			p="NVASTR(";
			w=YSTR;
			break;
		default:
			p="";
			w=YUNK;
			break;
		}
		PFO(",%s", p);
		tmp->w=w;
		if (w==YUNK)
			emit(tmp);
		else {
			PFO("&partmp%d,", i+e->tempfun*numpartemps);
			femit(tmp, ")");
		}
	}
	PFO(")");
}

totype(e, end)
Nodep	e;
int	end;	/* 0 for begin, 1 for end */
{
	if (end) {
		if (!SELF_CONV(e->t)) {
			if ((e->w==YINT||e->w==YNUM) && (e->y!=e->w))
				PFO(")");
			if ((e->w==YINT||e->w==YNUM) && (e->y==YSTR))
				PFO(")");
			if ((e->w==YINT||e->w==YNUM) && (e->y==YUNK))
				PFO(")");
			if (e->w==YSTR&&(e->y==YNUM||e->y==YINT||e->y==YUNK))
				PFO(")");
		}
	} else {
		if (!SELF_CONV(e->t)) {
			if (e->w==YINT && e->y!=YINT) PFO("((int)");
			if (e->w==YNUM && e->y!=YNUM) PFO("((double)");
			if ((e->w==YINT || e->w==YNUM) && e->y==YSTR)
				PFO("atof(");
			if ((e->w==YINT || e->w==YNUM) && e->y==YUNK)
				PFO("TONUM(");
			if (e->w==YSTR && e->y==YUNK)
				PFO("TS(");
			if (e->w==YSTR && e->y==YNUM)
				PFO("FTOA(tmpstr%d,", e->tempstr);
			if (e->w==YSTR && e->y==YINT)
				PFO("ITOA(tmpstr%d,", e->tempstr);
			else if (e->w==YUNK && e->y!=YUNK)
				ER "INTERNAL: = as expr confused awkcc" SX;
		}
	}
}

pa(e)
Nodep	e;
{
	PFO((e->t==ARRPARAM)?"Apa_%s":"Aa_%s", e->s);
}
