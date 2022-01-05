
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
extern VARINFOP	*symtab;
extern int	currpush;
extern int	dbg, verbose;

print_symtab()
{
	int	i, j;
	VARINFOP	tmp;

	for (i=0; tmp=symtab[i]; i++) {
		if (dbg || verbose) {
			fprintf(stderr,"%d)%s(%s),%d(currpush=%d,rt=%d,curr=%d).\n",
				i,tmp->func,tmp->string,tmp->type,currpush,
				(tmp->rt)[currpush],(tmp->cur)[currpush]);
			for (j=0; j<symtab[i]->ne; j++)
				printnode(tmp->exprs[j], 5);
		} else if (verbose)
			fprintf(stderr,"%d)%s(%s),%d.\n", i,tmp->func,tmp->string,tmp->type);
	}
}

arraytype()
{
	int	i,j, didwork=0;
	VARINFOP	tmp, tmp2;
	Nodep	n;
	extern uchar	*curfname;

	do {
		didwork=0;
		for (i=0; tmp=symtab[i]; i++) {
			if (tmp->atyped) continue;
			switch(tmp->type) {
			case ARRPARAM:
				tmp->atyped=1;
				for (j=0; j<tmp->ne; j++) {
					n=tmp->exprs[j];
					switch(n->t) {
					case ARG: case SYM:
						curfname=symtab[n->sympos]->func;
						setarrname(n);
						didwork=1;
						break;
					case ARRAY: case ARRPARAM: case SYSARRAY:
						if (n->LEFT)
							ER "Can't pass an array element (%s[]) as an array", n->s SX;
						break;
					default:
						ER "%s is not an array name", n->s SX;
					}
				}
				break;
			case PARAMETER:
				for (j=0; j<tmp->ne; j++) {
					n=tmp->exprs[j];
					switch(n->t) {
					case ARRAY: case ARRPARAM: case SYSARRAY:
						if (n->LEFT)
							tmp->type=VARPARAM;
						else tmp->type=ARRPARAM;
						didwork=1;
						break;
					case ARG:
						curfname=symtab[n->sympos]->func;
						tmp2=fnmemq(curfname, n->argnum);
						if (tmp2->type!=PARAMETER) {
							n->t=tmp2->type;
							didwork=1;
						}
						break;
					case SYM:
						if (tmp2=OVMEMQ(n->s)) {
							n->t=tmp2->type;
							add_var(n);
						} else if (tmp2=SAMEMQ(n->s))
							n->t=tmp2->type;
						else break;
						didwork=1;
						break;
					default: 
						tmp->type=VARPARAM;
						didwork=1;
						break;
					}
				}
				break;
			case VARPARAM:
				tmp->atyped=1;
				for (j=0; j<tmp->ne; j++) {
					n=tmp->exprs[j];
					switch(n->t) {
					case ARG: case SYM:
						curfname=symtab[n->sympos]->func;
						setvarname(n);
						didwork=1;
						break;
					case ARRAY: case ARRPARAM: case SYSARRAY:
						if (!n->LEFT)
							ER "Can't pass array %s as a variable", n->s SX;
						break;
					default: break;
					}
				}
				break;
			}
		}
	} while (didwork);
	/*  Anything leftover is a variable, on the grounds that it is
	    never used
	 */
	for (i=0; tmp=symtab[i]; i++) {
		if (tmp->type==PARAMETER) {
			tmp->type=VARPARAM;
			for (j=0; j<tmp->ne; j++) {
				switch(tmp->exprs[j]->t) {
				case ARG:
				case SYM: 
					setvarname(tmp->exprs[j]); 
					break;
				default: ER "INTERNAL: Can't happen" SX;
				}
			}
		}
	}
}

VARINFOP
add_sym(name, type, dnode, funcname, argnum)
uchar	*name;
int	type;
Nodep	dnode;
uchar	*funcname;
int	argnum;
{
	static int	siz;
	VARINFOP	tmp;

	if (!siz) {
		symtab=(VARINFOP *)calloc(++siz, sizeof(VARINFOP));
		symtab[siz-1]=(VARINFOP) NULL;
		return (VARINFOP) NULL;
	}
	symtab=(VARINFOP *)
	    realloc (symtab, (++siz)*sizeof(VARINFOP));
	tmp=symtab[siz-2]=make_varinfo();
	tmp->type=type;
	tmp->string=name;
	tmp->func=funcname;
	tmp->flags=0;
	tmp->sympos=siz-2;
	tmp->argnum=argnum;
	tmp->called=0;
	if (dnode)
		newsub(tmp, dnode);
	symtab[siz-1]=(VARINFOP) NULL;
	return tmp;
}

newsub(v, e)
VARINFOP	v;
Nodep	e;
{
	e->sympos=v->sympos;
	v->exprs=(Nodep *)realloc(v->exprs, sizeof(Nodep)*++v->ne);
	v->exprs[v->ne-1]=e;
}

add_doll(x)
Nodep	x;
{
	int	save;

	save=x->sympos;
	if (!ememq(x, INDIRECT))
		add_sym("", INDIRECT, x, "", 0);
	x->sympos=save;
}

add_regexp(n, anchored)
Nodep	n;
int	anchored;
{
	int	i;
	VARINFOP	tmp;

	if (tmp=RMEMQ(n->s, anchored))
		n->sympos=tmp->sympos;
	else add_sym(n->s, REGEXP, n, "", 0)->flags|=anchored;
}

add_uvar(var)
uchar	*var;
{
	if (!UMEMQ(var))
		add_sym(var, UVAR, (Nodep) NULL, "", 0);
}

add_fparam(param, func)
Nodep	param;	/* parameter */
uchar	*func;	/* function */
{
	VARINFOP	tmpp;
	Nodep	tmp;
	int	i;

	tmpp=fnmemq(func, param->argnum);
	if (!tmpp)
		tmpp=add_sym("", PARAMETER, (Nodep)NULL, func, param->argnum);
	/* newsub would overwrite sympos */
	i=param->sympos;
	newsub(tmpp, param);
	param->sympos=i;
	tmpp->called++;
}

add_param(param, func)
Nodep	param;	/* parameter */
uchar	*func;	/* function */
{
	int	i;
	VARINFOP	tmpp;

	for (i=1; i<param->argnum; i++)
		if (!strcmp(fnmemq(func, i)->string, param->s))
			ER "function %s defined with two identical parameters (%s)", func, param->s SX;
	if (tmpp=fnmemq(func, i))
		tmpp->string=param->s;
	else add_sym(param->s, PARAMETER, (Nodep) NULL, func, param->argnum);
}

add_array(sym)
uchar	*sym;
{
	int	i;

	if (!AMEMQ(sym)) {
		if (OVMEMQ(sym))
			ER "%s can't be both an array and a variable!", sym SX;
		else add_sym(sym, ARRAY, (Nodep) NULL, "", 0);
	}
}

add_var(var)
Nodep	var;
{
	VARINFOP	tmp;
	int	i;

	if (tmp=OVMEMQ(var->s))
		newsub(tmp, var);
	else add_sym(var->s, var->t, var, "", 0);
}

add_glob(f, n)
uchar	*f;
Nodep	n;
{
	VARINFOP	tmp;
	register int	i;

	tmp=FMEMQ(f);
	for (i=0; i<tmp->ne; i++)
		if (!strcmp(n->s, tmp->exprs[i]->s))
			return;
	newsub(tmp, n);
}

add_func(f)
uchar	*f;
{
	VARINFOP	tmpp;

	if (!(tmpp=FMEMQ(f)))
		add_sym(f, FUNC, (Nodep) NULL, "", 0);
	else tmpp->called++;
}


/*  Various predicates
 */

nodecmp(n1, n2)
VARINFOP	*n1, *n2;
{
	if ((*n1)->type==INDIRECT && (*n2)->type!=INDIRECT)
		return (1);
	else if ((*n1)->type!=INDIRECT && (*n2)->type==INDIRECT)
		return (-1);
	else if ((*n1)->type!=INDIRECT && (*n2)->type!=INDIRECT)
		return (0);
	else return((*n1)->exprs[0]->n-(*n2)->exprs[0]->n);
}

/* Given an fname and a param position, find the entry.
 */
VARINFOP
fnmemq(func, pos)
uchar	*func;	/* func name */
int	pos;	/* we're looking for parameter 'pos' */
{
	register VARINFOP	tmp, *sp;

	for (sp=symtab; tmp=(*sp); sp++)
		if (ISPARAM(tmp->type) &&
		    !strcmp(tmp->func, func) &&
		    tmp->argnum==pos)
			break;
	return (tmp);
}

/* Find an entry with the appropriate name, of this type.
 */
VARINFOP
memq(name, type1, type2, rtype)
uchar	*name;
int	type1;
int	type2;
int	rtype;
{
	register int	i;
	register VARINFOP	tmp;

	for (i=0; tmp=symtab[i]; i++)
		if ((tmp->type==type1 || tmp->type==type2) &&
		    !strcmp(tmp->string, name)) {
			if (type1==REGEXP && (tmp->flags&FREGEXP)!=rtype)
				continue;
			break;
		}
	return (tmp);
}

/* Is an expression a member?  Good for $(x+y)
 */

ememq(expr, type)
register Nodep	expr;
int	type;
{
	VARINFOP	tmp, *sp;
	Nodep	n1, n2;

	for (sp=symtab; tmp=(*sp); sp++) {
		n1=tmp->exprs[0];
		n2=expr;
		if ((tmp->type==type)	&&
		    (n1&&n2)		&&
		    (n1->t==n2->t)	&&
		    (n1->n==n2->n))
			break;
	}
	return (tmp?1:0);
}
