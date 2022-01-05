
/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/
#define PFO		printf

#define NUMERIC(op)	(op==FLOAT	||	\
			 op==INT)

#define ASSIGNOP(op)	(op==ASSIGN 	||	\
			 op==MULTEQ	||	\
		  	 op==ADDEQ 	||	\
		  	 op==SUBEQ  	||	\
		  	 op==DIVEQ  	||	\
		  	 op==POWEQ  	||	\
		  	 op==MODEQ)

#define ISPARAM(op)	(op==PARAMETER	||	\
			 op==ARRPARAM	||	\
			 op==VARPARAM)

#define EVALORD_TEMPSTR(x)\
			((x->t == PREINCR	||	\
			 x->t == PREDECR	||	\
			 x->t == POSTINCR	||	\
			 x->t == POSTDECR) 	&&	\
			 x->w == YSTR)

#define SELF_CONV(x)	(x == VAR	||	\
			 x == SYSVAR	||	\
			 x == SYSARRAY	||	\
			 x == INT	||	\
			 x == VARPARAM  ||	\
			 x == ARRPARAM  ||	\
			 x == PREINCR	||	\
			 x == PREDECR	||	\
			 x == POSTINCR	||	\
			 x == POSTDECR	||	\
			 x == INDIRECT	||	\
			 x == FLOAT	||	\
			 x == STRING	||	\
			 x == ARRAY)

#define FAKELCAT(x,n)	(x && 				\
			 x->narg[n] && 			\
			 x->narg[n]->t==STRING &&	\
			 !*(x->narg[n]->s) &&		\
			 x->subs==2)

#define FAKECAT(x)	(FAKELCAT(x,0) || FAKELCAT(x,1))

#define ISFUNC(x)	(x && x->t==FUNC)

#define ISC1(x)	(x==FOR||x==DO||x==WHILE||x==IF||x==INWHILE||x==CLINE)

#define ISC2(x)	(x == PASTAT || x == PASTAT2 || x == PROGRAM)

#define SELF_EVABORDING(x)	(ISC1(x)	||	\
				 ISC2(x)	||	\
				 x == RETURN	||	\
				 x == COMPSTMT	||	\
				 x == CONDEXPR)

#define ISARRAY(x)	(x == ARRAY	||		\
			 x == SYSARRAY	||		\
			 x == ARRPARAM)

#define OPTVAR(x)	(x == VAR || x == SYSVAR)

#define VARNAME(x)	(OPTVAR(x) || x==VARPARAM)

#define VARIABLE(x)	(x == PARAMETER	||		\
			 x == INDIRECT	||		\
			 x == VARPARAM  ||		\
			 OPTVAR(x)	|| 		\
			 ISARRAY(x))


#define OPTUNK(x)	((x->t==CAT&&!FAKECAT(x)) || 	\
			 x->t==FTOLOWER 	  || 	\
			 x->t==FTOUPPER)

#define CANDOUNK(x)	(OPTVAR(x->t) || VARIABLE(x->t) || x->t==CALL || OPTUNK(x))


#define CHDOLL(e)	(!NUMERIC(e->t)||e->n)

#define NISREC(x)	(x &&				\
			 x->t==INDIRECT && 		\
			 CHDOLL(e->LEFT))

#define ISREC(x)	(x &&				\
			 x->t==INDIRECT && 		\
			 (x->LEFT->t==INT || x->LEFT->t==FLOAT) &&	\
			 !x->LEFT->n)

#define SISL(e,st)	!strcmp(e->LEFT->s, st)

#define OVMEMQ(n)	memq(n, VAR, SYSVAR, 0)

#define SAMEMQ(n)	memq(n, SYSARRAY, ARRAY, 0)

#define AMEMQ(n)	memq(n, ARRAY, ARRAY, 0)

#define UMEMQ(n)	memq(n, UVAR, UVAR, 0)

#define FMEMQ(n)	memq(n, FUNC, FUNC, 0)

#define RMEMQ(n, t)	memq(n,	REGEXP, REGEXP, t)

#define ILLUM(s)	(illum || UMEMQ(s))

#define ER		sprintf(errbuf,

#define SX		), yyerror(errbuf)
