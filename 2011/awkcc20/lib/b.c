/*
Copyright (c) 1984, 19885, 1986, 1987 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/

//#define	DEBUG

#include "awk.h"
#define NIL ((Node *) 0)
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

#define	HAT	(NCHARS-1)	/* matches ^ in regular expr */
				/* NCHARS is 2**n */
#define MAXLIN 256

#define type(v)		(v)->nobj
#define left(v)		(v)->narg[0]
#define right(v)	(v)->narg[1]
#define parent(v)	(v)->nnext

#define LEAF	case CHAR: case DOT: case FINAL: case ALL:
#define UNARY	case STAR: case PLUS: case QUEST:

/* encoding in tree Nodes:
	leaf (CCL, NCCL, CHAR, DOT, FINAL, ALL):
		left is index, right contains value or pointer to value
	unary (STAR, PLUS, QUEST): left is child, right is null
	binary (CAT, OR): left and right are children
	parent contains pointer to parent
*/


uchar	*tostring();
uchar	chars[MAXLIN];
int	setvec[MAXLIN];
int	tmpset[MAXLIN];
Node	*point[MAXLIN];

int	rtok;		/* next token in current re */
int	rlxval;
uchar	*rlxstr;
uchar	*prestr;	/* current position in current re */
uchar	*lastre;	/* origin of last re */

static	int setcnt;
static	int poscnt;

uchar	*patbeg;
int	patlen;

#define	NFA	20	/* cache this many dynamic fa's */
fa	*fatab[NFA];
int	nfatab	= 0;	/* entries in fatab */
fa	*mdfa();

/* makedfa is a replacement for ins_regexp, which I used to use (bad,
 * because it caches an unlimited number of regular expressions).
 */

cleanfas()
{
	register int	i;

	for (i=0; i<nfatab; i++)
		freefa(fatab[i]);
}

fa *makedfa(s, anchor)	/* returns dfa for reg expr s */
	uchar *s;
	int anchor;
{
	int i, use, nuse;
	fa *pfa;

	for (i = 0; i < nfatab; i++)	/* is it there already? */
		if (fatab[i]->anchor == anchor && strcmp(fatab[i]->restr,s) == 0) {
			fatab[i]->use++;
			return fatab[i];
		}
	pfa = mdfa(reparse(s), anchor);
	/* this next line moved from below by jcr because s wasn't passed
	 * down
	 */
	pfa->restr = tostring(s);
	if (nfatab < NFA) {	/* room for another */
		fatab[nfatab] = pfa;
		fatab[nfatab]->use = 1;
		nfatab++;
		return pfa;
	}
	use = fatab[0]->use;	/* replace least-recently used */
	nuse = 0;
	for (i = 1; i < nfatab; i++)
		if (fatab[i]->use < use) {
			use = fatab[i]->use;
			nuse = i;
		}
	freefa(fatab[nuse]);
	fatab[nuse] = pfa;
	pfa->use = 1;
	return pfa;
}

fa *mdfa(p, anchor)	/* does the real work of making a dfa */
	Node *p;
	int anchor;	/* anchor = 1 for anchored matches, else 0 */
{
	Node *p1, *reparse();
	fa *f;

	p1 = op2(CAT, op2(STAR, op2(ALL, NIL, NIL), NIL), p);
		/* put ALL STAR in front of reg.  exp. */
	p1 = op2(CAT, p1, op2(FINAL, NIL, NIL));
		/* put FINAL after reg.  exp. */

	poscnt = 0;
	penter(p1);	/* enter parent pointers and leaf indices */
	if ((f = (fa *) Calloc(1, sizeof(fa) + poscnt*sizeof(rrow))) == NULL)
		overflo("no room for fa");
	f->accept = poscnt-1;	/* penter has computed number of positions in re */
	cfoll(f, p1);	/* set up follow sets */
	freetr(p1);
	if ((f->posns[0] = (int *) Calloc(1, *(f->re[0].lfollow)*sizeof(int))) == NULL)
			overflo("out of space in mdfa");
	if ((f->posns[1] = (int *) Calloc(1, sizeof(int))) == NULL)
		overflo("out of space in mdfa");
	*f->posns[1] = 0;
	f->initstat = minit(f, anchor);
	f->anchor = anchor;
	return f;
}

int minit(f, anchor)
	fa *f;
	int anchor;
{
	register int i, k;

	f->curstat = 2;
	f->out[2] = 0;
	f->reset = 0;
	k = *(f->re[0].lfollow);
	xfree(f->posns[2]);			
	if ((f->posns[2] = (int *) Calloc(1, (k+1)*sizeof(int))) == NULL)
		overflo("out of space in minit");
	for (i=0; i<=k; i++) {
		(f->posns[2])[i] = (f->re[0].lfollow)[i];
	}
	if ((f->posns[2])[1] == f->accept)
		f->out[2] = 1;
	for (i=0; i<NCHARS; i++)
		f->gototab[2][i] = 0;
	f->curstat = cgoto(f, 2, HAT);
	if (anchor) {
		*f->posns[2] = k-1;	/* leave out position 0 */
		for (i=0; i<k; i++) {
			(f->posns[0])[i] = (f->posns[2])[i];
		}

		f->out[0] = f->out[2];
		if (f->curstat != 2)
			--(*f->posns[f->curstat]);
	}
	return f->curstat;
}

penter(p)	/* set up parent pointers and leaf indices */
	Node *p;
{
	switch(type(p)) {
	case CCL:
	case NCCL:
	LEAF
		left(p) = (Node *) poscnt;
		point[poscnt++] = p;
		break;
	UNARY
		penter(left(p));
		parent(left(p)) = p;
		break;
	case CAT:
	case OR:
		penter(left(p));
		penter(right(p));
		parent(left(p)) = p;
		parent(right(p)) = p;
		break;
	default:
		awkerr( "unknown type %d in penter", type(p) );
		break;
	}
}

freetr(p)	/* free parse tree */
	Node *p;
{
	switch (type(p)) {
	case CCL:
	case NCCL:
	LEAF
		xfree(p);
		break;
	UNARY
		freetr(left(p));
		xfree(p);
		break;
	case CAT:
	case OR:
		freetr(left(p));
		freetr(right(p));
		xfree(p);
		break;
	default:
		awkerr( "unknown type %d in freetr", type(p) );
		break;
	}
}

uchar *cclenter(p)
	uchar *p;
{
	register int i, c;
	uchar *op;
	int	upper_bound;

	op = p;
	i = 0;
	while ((c = *p++) != 0) {
		if (c == '\\') {
			c = get_octal(&p);
		} else if (c == '-' && i > 0 && chars[i-1] != 0) {
			if (*p != 0) {
				c = chars[i-1];
				if (*p=='\\') {
					p++;
					upper_bound=get_octal(&p);
				} else upper_bound=(*p);
				while (c < upper_bound) {
					if (i >= MAXLIN-1)
						overflo("character class too big");
					chars[i++] = ++c;
				}
				if (upper_bound==*p)
					p++;
			}
		}
		if (i >= MAXLIN-1)
			overflo("character class too big");
		chars[i++] = c;
	}
	chars[i++] = '\0';
	/* dprintf( ("cclenter: in = |%s|, out = |%s|\n", op, chars) ); */
	xfree(op);
	return(tostring(chars));
}

get_octal(p)
uchar	**p;
{
	int	c;

	c=(**p);
	(*p)++;
	if (c=='t')
		c = '\t';
	else if (c == 'n')
		c = '\n';
	else if (c == 'f')
		c = '\f';
	else if (c == 'r')
		c = '\r';
	else if (c == 'b')
		c = '\b';
	else if (c == '\\')
		c = '\\';
	else if (isdigit(c)) {
		int n = c - '0';
		if (isdigit(**p)) {
			n = 8 * n + **p - '0';
			(*p)++;
			if (isdigit(**p)) {
				n = 8 * n + **p - '0';
				(*p)++;
			}
		}
		c = n;
	} /* else */
		/* c = c; */
	return c;
}

overflo(s)
	uchar *s;
{
	awkerr( "regular expression too big: %s", s );
}

cfoll(f, v)	/* enter follow set of each leaf of vertex v into lfollow[leaf] */
	fa *f;
	register Node *v;
{
	register int i;
	register int *p;

	switch(type(v)) {
	case CCL:
	case NCCL:
	LEAF
		f->re[(int) left(v)].ltype = type(v);
		f->re[(int) left(v)].lval = (int) right(v);
		for (i=0; i<=f->accept; i++)
			setvec[i] = 0;
		setcnt = 0;
		follow(v);	/* computes setvec and setcnt */
		if ((p = (int *) Calloc(1, (setcnt+1)*sizeof(int))) == NULL)
			overflo("follow set overflow");
		f->re[(int) left(v)].lfollow = p;
		*p = setcnt;
		for (i = f->accept; i >= 0; i--)
			if (setvec[i] == 1) *++p = i;
		break;
	UNARY
		cfoll(f,left(v));
		break;
	case CAT:
	case OR:
		cfoll(f,left(v));
		cfoll(f,right(v));
		break;
	default:
		awkerr( "unknown type %d in cfoll", type(v) );
	}
}

first(p)		/* collects initially active leaves of p into setvec */
	register Node *p;	/* returns 0 or 1 depending on whether p matches empty string */
{
	register int b;

	switch(type(p)) {
	case CCL:
	case NCCL:
	LEAF
		if (setvec[(int) left(p)] != 1) {
			setvec[(int) left(p)] = 1;
			setcnt++;
		}
		if (type(p) == CCL && (*(uchar *) right(p)) == '\0')
			return(0);		/* empty CCL */
		else return(1);
	case PLUS:
		if (first(left(p)) == 0) return(0);
		return(1);
	case STAR:
	case QUEST:
		first(left(p));
		return(0);
	case CAT:
		if (first(left(p)) == 0 && first(right(p)) == 0) return(0);
		return(1);
	case OR:
		b = first(right(p));
		if (first(left(p)) == 0 || b == 0) return(0);
		return(1);
	}
	awkerr( "unknown type %d in first", type(p) );
	return(-1);
}

follow(v)
	Node *v;		/* collects leaves that can follow v into setvec */
{
	Node *p;

	if (type(v) == FINAL)
		return;
	p = parent(v);
	switch (type(p)) {
	case STAR:
	case PLUS:
		first(v);
		follow(p);
		return;

	case OR:
	case QUEST:
		follow(p);
		return;

	case CAT:
		if (v == left(p)) {	/* v is left child of p */
			if (first(right(p)) == 0) {
				follow(p);
				return;
			}
		}
		else		/* v is right child */
			follow(p);
		return;
	}
}

awkmember(c, s)	/* is c in s? */
	register uchar c, *s;
{
	while (*s)
		if (c == *s++)
			return(1);
	return(0);
}


match(f, p)
	register fa *f;
	register uchar *p;
{
	register int s, ns;

	s = f->reset?minit(f,0):f->initstat;
	if (f->out[s])
		return(1);
	do {
		if (ns=f->gototab[s][*p])
			s=ns;
		else
			s=cgoto(f,s,*p);
		if (f->out[s])
			return(1);
	} while (*p++ != 0);
	return(0);
}

pmatch(f, p)
	register fa *f;
	register uchar *p;
{
	register int s, ns;
	register uchar *q;
	int i, k;

	s = f->reset?minit(f,1):f->initstat;
	patbeg = p;
	patlen = -1;
	do {
		q = p;
		do {
			if (f->out[s])		/* final state */
				patlen = q-p;
			if (ns=f->gototab[s][*q])
				s=ns;
			else
				s=cgoto(f,s,*q);
			if (s==1)	/* no transition */
				if (patlen >= 0) {
					patbeg = p;
					return(1);
				}
				else
					goto nextin;	/* no match */
		} while (*q++ != 0);
		if (f->out[s])
			patlen = q-p-1;	/* don't count $ */
		if (patlen >= 0) {
			patbeg = p;
			return(1);
		}
	nextin:
		s = 2;
		if (f->reset) {
			for (i=2; i<=f->curstat; i++)
				xfree(f->posns[i]);
			k = *f->posns[0];			
			if ((f->posns[2] = (int *) Calloc(1, (k+1)*sizeof(int))) == NULL)
				overflo("out of space in pmatch");
			for (i=0; i<=k; i++)
				(f->posns[2])[i] = (f->posns[0])[i];
			f->initstat = f->curstat = 2;
			f->out[2] = f->out[0];
			for (i=0; i<NCHARS; i++)
				f->gototab[2][i] = 0;
		}
	} while (*p++ != 0);
	return (0);
}

nematch(f, p)
	register fa *f;
	register uchar *p;
{
	register int s, ns;
	register uchar *q;
	int i, k;

	s = f->reset?minit(f,1):f->initstat;
	patlen = -1;
	while (*p) {
		q = p;
		do {
			if (f->out[s])		/* final state */
				patlen = q-p;
			if (ns=f->gototab[s][*q])
				s=ns;
			else
				s=cgoto(f,s,*q);
			if (s==1)	/* no transition */
				if (patlen > 0) {
					patbeg = p;
					return(1);
				}
				else
					goto nnextin;	/* no nonempty match */
		} while (*q++ != 0);
		if (f->out[s])
			patlen = q-p-1;	/* don't count $ */
		if (patlen > 0 ) {
			patbeg = p;
			return(1);
		}
	nnextin:
		s = 2;
		if (f->reset) {
			for (i=2; i<=f->curstat; i++)
				xfree(f->posns[i]);
			k = *f->posns[0];			
			if ((f->posns[2] = (int *) Calloc(1, (k+1)*sizeof(int))) == NULL)
				overflo("out of state space");
			for (i=0; i<=k; i++)
				(f->posns[2])[i] = (f->posns[0])[i];
			f->initstat = f->curstat = 2;
			f->out[2] = f->out[0];
			for (i=0; i<NCHARS; i++)
				f->gototab[2][i] = 0;
		}
	p++;
	}
	return (0);
}

Node *regexp(), *primary(), *concat(), *alt(), *unary();

Node *reparse(p)
	uchar *p;
{
	/* parses regular expression pointed to by p */
	/* uses relex() to scan regular expression */
	Node *np;

	/* dprintf( ("reparse <%s>\n", p) ); */
	lastre = prestr = p;	/* prestr points to string to be parsed */
	rtok = relex();
	if (rtok == '\0')
		awkerr( "empty regular expression" );
	np = regexp();
	if (rtok == '\0')
		return(np);
	else
		awkerr( "syntax error in regular expression %s at %s", lastre, prestr );
	/*NOTREACHED*/
}

Node *regexp()
{
	return (alt(concat(primary())));
}

Node *primary()
{
	Node *np;

	switch (rtok) {
	case CHAR:
		np = op2(CHAR, NIL, (Node *) rlxval);
		rtok = relex();
		return (unary(np));
	case ALL:
		rtok = relex();
		return (unary(op2(ALL, NIL, NIL)));
	case DOT:
		rtok = relex();
		return (unary(op2(DOT, NIL, NIL)));
	case CCL:
		np = op2(CCL, NIL, (Node*) cclenter(rlxstr));
		rtok = relex();
		return (unary(np));
	case NCCL:
		np = op2(NCCL, NIL, (Node *) cclenter(rlxstr));
		rtok = relex();
		return (unary(np));
	case '^':
		rtok = relex();
		return (unary(op2(CHAR, NIL, (Node *) HAT)));
	case '$':
		rtok = relex();
		return (unary(op2(CHAR, NIL, NIL)));
	case '(':
		rtok = relex();
		if (rtok == ')') {	/* special pleading for () */
			rtok = relex();
			return unary(op2(CCL, NIL, (Node *) tostring("")));
		}
		np = regexp();
		if (rtok == ')') {
			rtok = relex();
			return (unary(np));
		}
		else
			awkerr( "syntax error in regular expression %s at %s", lastre, prestr );
	default:
		awkerr( "illegal primary in regular expression %s at %s", lastre, prestr );
	}
	/*NOTREACHED*/
}

Node *concat(np)
	Node *np;
{
	switch (rtok) {
	case CHAR: case DOT: case ALL: case CCL: case NCCL: case '$': case '(':
		return (concat(op2(CAT, np, primary())));
	default:
		return (np);
	}
}

Node *alt(np)
	Node *np;
{
	if (rtok == OR) {
		rtok = relex();
		return (alt(op2(OR, np, concat(primary()))));
	}
	return (np);
}

Node *unary(np)
	Node *np;
{
	switch (rtok) {
	case STAR:
		rtok = relex();
		return (unary(op2(STAR, np, NIL)));
	case PLUS:
		rtok = relex();
		return (unary(op2(PLUS, np, NIL)));
	case QUEST:
		rtok = relex();
		return (unary(op2(QUEST, np, NIL)));
	default:
		return (np);
	}
}

relex()		/* lexical analyzer for reparse */
{
	register int c;
	uchar cbuf[150];
	int clen, cflag;

	switch (c = *prestr++) {
	case '|': return OR;
	case '*': return STAR;
	case '+': return PLUS;
	case '?': return QUEST;
	case '.': return DOT;
	case '\0': prestr--; return '\0';
	case '^':
	case '$':
	case '(':
	case ')':
		return c;
	case '\\':
		if ((c = *prestr++) == 't')
			c = '\t';
		else if (c == 'n')
			c = '\n';
		else if (c == 'f')
			c = '\f';
		else if (c == 'r')
			c = '\r';
		else if (c == 'b')
			c = '\b';
		else if (c == '\\')
			c = '\\';
		else if (isdigit(c)) {
			int n = c - '0';
			if (isdigit(*prestr)) {
				n = 8 * n + *prestr++ - '0';
				if (isdigit(*prestr))
					n = 8 * n + *prestr++ - '0';
			}
			c = n;
		} /* else it's now in c */
		rlxval = c;
		return CHAR;
	default:
		rlxval = c;
		return CHAR;
	case '[': 
		clen = 0;
		if (*prestr == '^') {
			cflag = 1;
			prestr++;
		}
		else {
			cflag = 0;
		}
		for (;;) {
			if ((c = *prestr++) == '\\') {
				cbuf[clen++] = '\\';
				if ((c = *prestr++) == '\0')
					awkerr( "nonterminated character class %s", lastre );
				cbuf[clen++] = c;
			} else if (c == ']') {
				cbuf[clen] = 0;
				rlxstr = tostring(cbuf);
				if (cflag == 0)
					return CCL;
				else
					return NCCL;
			} else if (c == '\n') {
				awkerr( "newline in character class %s...", lastre );
			} else if (c == '\0') {
				awkerr( "nonterminated character class %s", lastre );
			} else
				cbuf[clen++] = c;
		}
	}
}

int cgoto(f, s, c)
	fa *f;
	int s, c;
{
	register int i, j, k;
	register int *p, *q;

	for (i=0; i<=f->accept; i++)
		setvec[i] = 0;
	setcnt = 0;
	/* compute positions of gototab[s,c] into setvec */
	p = f->posns[s];
	for (i=1; i<=*p; i++) {
		if ((k = f->re[p[i]].ltype) != FINAL) {
			if (k == CHAR && c == f->re[p[i]].lval
				|| k == DOT && c != 0 && c != HAT
				|| k == ALL && c != 0
				|| k == CCL && awkmember(c, (uchar *) f->re[p[i]].lval)
				|| k == NCCL && !awkmember(c, (uchar *) f->re[p[i]].lval) && c != 0 && c != HAT) {
					q = f->re[p[i]].lfollow;
					for (j=1; j<=*q; j++) {
						if (setvec[q[j]] == 0) {
							setcnt++;
							setvec[q[j]] = 1;
						}
					}
				}
		}
	}
	/* determine if setvec is a previous state */
	tmpset[0] = setcnt;
	j = 1;
	for (i = f->accept; i >= 0; i--)
		if (setvec[i]) {
			tmpset[j++] = i;
		}
	/* tmpset == previous state? */
	for (i=1; i<= f->curstat; i++) {
		p = f->posns[i];
		if ((k = tmpset[0]) != p[0])
			goto different;
		for (j = 1; j <= k; j++)
			if (tmpset[j] != p[j])
				goto different;
		/* setvec is state i */
		f->gototab[s][c] = i;
		return i;
	different:;
	}

	/* add tmpset to current set of states */
	if (f->curstat >= NSTATES-1) {
		f->curstat = 2;
		f->reset = 1;
		for (i=2; i<NSTATES; i++)
			xfree(f->posns[i]);
	}
	else
		++(f->curstat);
	for (i=0; i<NCHARS; i++)
		f->gototab[f->curstat][i] = 0;
	xfree(f->posns[f->curstat]);
	if ((p = (int *) Calloc(1, (setcnt+1)*sizeof(int))) == NULL)
		overflo("out of space in cgoto");

	f->posns[f->curstat] = p;
	f->gototab[s][c] = f->curstat;
	for (i = 0; i <= setcnt; i++)
		p[i] = tmpset[i];
	if (setvec[f->accept])
		f->out[f->curstat] = 1;
	else
		f->out[f->curstat] = 0;
	return f->curstat;
}


freefa(f)
	struct fa *f;
{

	register int i;

	if (f == NULL)
		return;
	for (i=0; i<=f->curstat; i++)
		xfree(f->posns[i]);
	for (i=0; i<=f->accept; i++) {
		xfree(f->re[i].lfollow);
		/* memory leak fix? */
		if (f->re[i].ltype==CCL || f->re[i].ltype==NCCL)
			xfree(f->re[i].lval);
	}
	/* end fix */
	xfree(f->restr);
	xfree(f);
}
