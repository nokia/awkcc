/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <ctype.h>
#include <stdio.h>
#include "awkcc.h"
#include <stdarg.h>

standard_string(t, p, w)
int	t;
char	**p;
int	*w;
{
	switch(t) {
	case YSTR: *p="STR"; *w=YSTR; break;
	case YINT:
	case YNUM: *p="NUM"; *w=YNUM; break;
	default: *p="VARS"; *w=YUNK; break;
	}
}

catleaves(e)
Nodep	e;
{
	if (e->t==CAT)
		return catleaves(e->LEFT)+catleaves(e->RIGHT);
	else return 1;
}

pemitcatleaves(e)
Nodep	e;
{
	if (e->t == CAT) {
		pemitcatleaves(e->LEFT);
		pemitcatleaves(e->RIGHT);
	} else memit("fputs(", e, ",pfp);\n");
}

emitcatleaves(e, num)
Nodep	e;
int	num;
{
	if (e->t != CAT) {
		emit(e);
		if (num>1)
			PFO(", ");
		return --num;
	} else {
		num=emitcatleaves(e->LEFT, num);
		return emitcatleaves(e->RIGHT, num);
	}
}

static int	cflag, eflag, rflag;
regstr(s)
uchar	*s;
{
	int	i;
	uchar	c;
	
	cflag=eflag=rflag=0;
	for (i=0; c=(*s++); i++) {
		switch (c) {
		case '^':
			if (!i)	
				cflag=1;
			break;
		case '|':
			eflag=1;
			rflag=1;
			break;
		case '(': case ')': case '$': case '[':
		case ']': case '?': case '+': case '.':
		case '*':
			rflag=1;
			break;
		case '\\':
			if (*s) s++;
			break;
		}
	}
	if (eflag)
		return RREGEXP;
	else if (rflag)
		return RREGEX;
	else if (cflag) 
		return RCARAT;
	else return RNORMAL;
}

Nodep	exptostat(a)
Nodep	a;
{
	a->ntype = 1;
	return(a);
}

Nodep	linkum(a,b)
Nodep	a,b;
{
	register Nodep	c;

	if (!(a && b)) return (a?a:b);
	for (c=a; c->nnext != NULL; c=c->nnext)
		;
	c->nnext=b;
	return(a);
}

bracecheck()
{
	extern int	bracecnt, brackcnt, parencnt;
	int	c;
	static int	beenhere=0;

	if (beenhere++)
		return;
	while ((c = input()) != EOF && c != '\0')
		bclass(c);
	bcheck2(bracecnt, '{', '}');
	bcheck2(bracecnt, '[', ']');
	bcheck2(bracecnt, '(', ')');
}

bcheck2(n, c1, c2)
{
	if (n == 1)
		fprintf(stderr, "\tmissing %c\n", c2);
	else if (n > 1)
		fprintf(stderr, "\t%d missing %c's\n", n, c2);
	else if (n == -1)
		fprintf(stderr, "\textra %c\n", c2);
	else if (n < -1)
		fprintf(stderr, "\t%d extra %c's\n", -n, c2);
}

bclass(c)
{
	extern int	bracecnt, brackcnt, parencnt;

	switch (c) {
	case '{': bracecnt++; break;
	case '}': bracecnt--; break;
	case '[': brackcnt++; break;
	case ']': brackcnt--; break;
	case '(': parencnt++; break;
	case ')': parencnt--; break;
	}
}

Nodep	rectonode()
{
	Nodep	retval, tmp;

	tmp=xmakenode(NUMBER, "0", 0.0);
	retval=op1(INDIRECT, tmp);
	return(retval);
}

isarg(s)
uchar	*s;
{
	extern Node	*arglist;
	Node	*p = arglist;
	int	n;

	for (n=1; p; p=p->nnext, n++)
		if (!strcmp(p->s, s))
			return n;
	return -1;
}

Nodep	defn(t1, t2, t3)
Nodep	t1,t2,t3;
{
	subsize(t1, 2);
	t1->LEFT=t2;	/* arglist */
	t1->RIGHT=compstmt(t3);	/* body */
	t1->t=FUNC;
	fix(t1);
	return(t1);
}

Nodep	setarrname(e)
Nodep	e;
{
	switch(e->t) {
	case SYM:
		e->t=ARRAY;
		break;
	case ARG:
		e->t=ARRPARAM;
		break;
	default:
		ER "Illegal  array %s", e->s SX;
	}
	fix(e);
	return(e);
}

setvarname(e)
Nodep	e;
{
	switch(e->t) {
	case SYM:
		e->t=VAR;
		break;
	case ARG:
		e->t=VARPARAM;
		break;
	default:
		return;
	}
	fix(e);
}

setfname(e)
Nodep	e;
{
	extern uchar	*curfname;

	curfname=e->s;
	add_func(e->s);
}

printstring(s, reg, iscarat)
uchar	*s;
int	reg;
int	iscarat;
{
	PFO("\"");
	if (iscarat) s++;
	for (; *s; s++) {
		switch(*s) {
		default: PFO("%c",*s); break;
		case '\n': PFO("\\n"); break;
		case '\t': PFO("\\t"); break;
		case '\f': PFO("\\f"); break;
		case '\r': PFO("\\r"); break;
		case '\b': PFO("\\b"); break;
		case '"': PFO("\\\""); break;
		case '\\':
			if (reg) PFO("\\\\");
			else {
				switch(*(s+1)) {
				case '"':
					PFO("\\\"");
					s++;
					break;
				case '\\':
					PFO("\\\\");
					s++;
					break;
				case 'n': PFO("\\n"); s++; break;
				case 't': PFO("\\t"); s++; break;
				case 'f': PFO("\\f"); s++; break;
				case 'r': PFO("\\r"); s++; break;
				case 'b': PFO("\\b"); s++; break;
				}
			}
			break;
		}
	}
	PFO("\"");
}

setfargnames(e)
register Nodep	e;
{
	int	i, j;
	uchar	*fname;

	fname=e->s;
	e=e->LEFT;
	for (i=1; e; e=e->nnext, i++) {
		j=e->argnum;
		e->argnum=i;
		add_fparam(e, fname);
		e->argnum=j;
	}
}

setargnames(e)
register Nodep	e;
{
	extern uchar	*curfname;
	register int	i;

	for (i=1; e; e=e->nnext, i++) {
		e->argnum=i;
		add_param(e, curfname);
		e->t=PARAMETER;
	}
}

wassysvar(s)
uchar	*s;
{
	if (!strcmp(s, "ARGC")	||
	    !strcmp(s, "FILENAME") ||
	    !strcmp(s, "FNR")	||
	    !strcmp(s, "FS")	||
	    !strcmp(s, "RLENGTH")	||
	    !strcmp(s, "RSTART")	||
	    !strcmp(s, "NF")	||
	    !strcmp(s, "NR")	||
	    !strcmp(s, "OFMT")	||
	    !strcmp(s, "SUBSEP")	||
	    !strcmp(s, "OFS")	||
	    !strcmp(s, "ORS")	||
	    !strcmp(s, "RS"))
		return(1);
	return(0);
}

foptvars(v)
VARINFOP	v;
{
	register int	i;

	for (i=0; i<v->ne; i++)
		foptvar(v->exprs[i]);
}

foptvar(e)
register Nodep	e;
{
	register VARINFOP	tmp;

	if (!e) return 0;
	if (OPTVAR(e->t) && (tmp=memq(e->s, INTVAR, FLOATVAR, 0))) {
		switch(tmp->type) {
		case INTVAR:
			e->t=INTVAR;
			e->y=YINT;
			return (1);
			break;
		case FLOATVAR:
			e->t=FLOATVAR;
			e->y=YNUM;
			return (1);
			break;
		}
	}
	return (0);
}

static int been_here = 0;
eprint()
{
	uchar *p, *q;
	int c;
	extern int compile_time;
	extern uchar	ebuf[300], *ep;

	if (!compile_time || been_here++ > 0)
		return;
	p = ep-1;
	if (p > ebuf && *p == '\n')
		p--;
	for (; p > ebuf && *p != '\n' && *p; p--)
		;
	while (*p == '\n')
		p++;
	fprintf(stderr, " context is\n\t");
	for (q=ep-1; q>=p && *q!=' ' && *q!='\t' && *q!='\n'; q--)
		;
	while (p < q)
		putc(*p++, stderr);
	fprintf(stderr, " >>> ");
	while (p < ep)
		putc(*p++, stderr);
	fprintf(stderr, "<<< ");
	if (*ep)
		while ((c=input()) !='\n' && c && c!=EOF) {
			putc(c, stderr);
			bclass(c);
		}
	putc('\n', stderr);
	ep=ebuf;
}

pdbug(line)
int	line;
{
	static int	prevline;
	extern uchar	*infilename;

	if (line && line!=prevline)
		PFO("\n# line %d \"%s\"\n", line, infilename);
	prevline=line;
}

Nodep
genstat(int stat, int num, int type, ...)
{
	Nodep	retval;
	va_list	ap;
	int	i;

	retval=makenode(type, -107, (TOKENP) NULL);
	retval->ntype=(short) stat;
	subsize(retval, ((num>0)?num:1));
	va_start(ap,type);
	for (i=0; i<num; i++)
		retval->narg[i]=va_arg(ap, Nodep);
	va_end(ap);
	fix(retval);
	return(retval);
}

deltst(e)
Nodep	e;
{
	Nodep	delete;
	Nodep	intest;

	if (!e) return 1;
	if (e->t!=INWHILE) return 2;
	if (!e->LEFT)	return 3;
	if (e->LEFT->t!=INTEST)	return 4;
	if (!e->RIGHT)	return 5;
	if (e->RIGHT->t != COMPSTMT)	return 6;
	if (nextlen(e->RIGHT) != 2) return 7;
	if (e->RIGHT->nnext->t != DELETE)	return 8;
	intest=e->LEFT;
	delete=e->RIGHT->nnext;
	if (nextlen(delete->LEFT->LEFT)!=1) return 9;
	if (strcmp(intest->LEFT->s, delete->LEFT->LEFT->s)) return 10;
	if (strcmp(intest->RIGHT->s, delete->LEFT->s)) return 11;
	return 0;
}

#define	WS(s)	(*s && (*s==' ' || *s=='\t'))
#define SKIPW	for (; WS(s); s++)
#define GETW	bufp=buf; while (*s && !WS(s)) *bufp++=(*s++); *bufp='\0'
#define EWARN	{eprint(); been_here--;}
#define EXCESS	if (*s && *s!='#') {\
			fprintf(stderr, "WARNING: some awkcchint ignored;\n");\
			EWARN;\
		}

hints(s)
char	*s;
{
	char	buf[BUFSIZ], *bufp, c;

	SKIPW;
	if (*s=='-') {
		c=(*++s); s++; GETW;
		switch(c) {
		case 'J': 
		case 'b': 
		case 'B': 
		case 'D':
		case 'U':
			process_args(c, "", 1);
			fprintf(stderr, "REMINDER:  assuming `-%c'\n", c);
			SKIPW;
			EXCESS;
			break;
		case 'u':
		case 'o':
		case 'c':
			SKIPW;
			GETW;
			process_args(c, tostring(buf), 1);
			fprintf(stderr, "REMINDER:  assuming `-%c %s'\n", c, buf);
			EXCESS;
			break;
		default:
			fprintf(stderr, "WARNING: invalid awkcchint;");
			EWARN;
			break;
		}
	} else {
		fprintf(stderr, "WARNING: Invalid awkcchint;");
		EWARN;
	}
}
