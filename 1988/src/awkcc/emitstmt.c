
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

extern VARINFOP	*symtab;
extern int	lineno;
extern int	bufferflag;
extern int	hasORS;
extern int	hasOFS;
static int	looptype;
static int	inuniq;

emitstmt(e)
Nodep	e;
{
	char	*p;
	int	i;
	int	end;
	Nodep	tmp;
	int	saveloop;

	if (!e) return;
	lineno=e->lineno;
	switch(e->t) {
	case CLINE:
		PFO("%s\n", e->s);
		break;
	case PASTAT2:
		PFO("if (ifelse%d || ", e->tempif);
		evalord(e->LEFT);
		emit(e->LEFT);
		PFO(")\nifelse%d=1;\n", e->tempif);
		droprets(e->LEFT, 0, FALSE);
		PFO("if (ifelse%d)\n{\n", e->tempif);
		emit(e->narg[2]);
		droprets(e->narg[2], 0, FALSE);
		PFO("}\nif (ifelse%d && ", e->tempif);
		evalord(e->RIGHT);
		emit(e->RIGHT);
		PFO(")\nifelse%d=0;\n", e->tempif);
		droprets(e->RIGHT, 0, FALSE);
		emit(e->nnext);
		break;
	case PASTAT:
		if (e->LEFT) {
			PFO("if (");
			evalord(e->LEFT);
			femit(e->LEFT, ")\n{\n");
			droprets(e->LEFT, 0, FALSE);
			emit(e->RIGHT);
			PFO("}\n");
			droprets(e->LEFT, 0, TRUE);
		} else memit("{\n", e->RIGHT, "}\n");
		emit(e->nnext);
		break;
	case PRINT:
		prefuncall(e);
		efile2(e->THREE, (int) e->RIGHT);
		end=0;
		for (tmp=e->LEFT; tmp; tmp=tmp->nnext) {
			end=!tmp->nnext;
			switch(tmp->w) {
			case YINT:
				if (hasORS || hasOFS) p="%s";
				else if (end) p="\\n";
				else p=" ";
				PFO("fprintf(pfp, \"%%d%s\",", p);
				emit(tmp);
				if (!(hasORS || hasOFS)) p="";
				else if (end) p=",ORS";
				else p=",OFS";
				PFO("%s)%s\n", p, end?"":";");
				break;
			case YNUM:
			case YNUMUNK:
				memit("fprintf(pfp,OFMT,", tmp, ");\n");
				putend(end, hasORS, hasOFS);
				break;
			case YSTR:
			case YSTRUNK:
				if (ISREC(tmp))
					PFO("printD0(pfp);\n");
				else if (tmp->t==CAT)
					pemitcatleaves(tmp);
				else memit("fputs(", tmp, ",pfp);\n");
				putend(end, hasORS, hasOFS);
				break;
			default:
				tmp->w=YSTR;
				memit("fputs(", tmp, ",pfp);\n");
				putend(end, hasORS, hasOFS);
				break;
			}
		}
		if (bufferflag && e->THREE)
			PFO(";\nfflush(pfp)");
		break;	
	case IF:
		/*  JCR - deal with funcalls in CONDEXPR! */
		PFO("if (");
		evalord(e->LEFT);
		femit(e->LEFT, ")\n{\n");
		droprets(e->LEFT, 0, FALSE);
		evalord(e->RIGHT);
		emit(e->RIGHT);
		droprets(e->RIGHT, 0, FALSE);
		PFO("}\n");
		if (e->THREE) {
			PFO("else\n{\n");
			droprets(e->LEFT, 0, FALSE);
			evalord(e->narg[2]);
			emit(e->narg[2]);
			droprets(e->narg[2], 0, FALSE);
			PFO("}\n");
		}
		droprets(e->LEFT, 0, TRUE);
		break;
	case INWHILE:
		saveloop=looptype; looptype=(++inuniq);
		if (!deltst(e)) {
			e->LEFT->LEFT->w=YUNK;
			PFO("wclear_htb(");
			pa(e->LEFT->RIGHT);
			memit(",", e->LEFT->LEFT, ");\n");
			looptype=saveloop;
			break;
		}
		i=arrpush(e->LEFT);
		PFO("{\n");
		PFO("int\tj%d;\n", i);
		PFO("int\tl%d;\n", i);
		PFO("ELEMP\telem%d;\n", i);
		PFO("ELEMP\tbkt%d;\n", i);
		PFO("j%d=0;\n",i);
		PFO("elem%d=((ELEMP)NULL);\n",i);
		PFO("l%d=",i);
		/* could use something uniq, I suppose*/
		pa(e->LEFT->RIGHT);
		PFO("->num;\nfor (;j%d<", i);
		pa(e->LEFT->RIGHT);
		PFO("->len&&!(bkt%d=",i);
		pa(e->LEFT->RIGHT);
		PFO("->tab[j%d]);j%d++)\n;\n",i,i);
		PFO("for (;;) {\n");
		PFO("if (!l%d)\nbreak;\n", i);
		PFO("else if (bkt%d) {\n", i);
		PFO("l%d--;\n", i);
		PFO("elem%d=bkt%d;\n", i, i);
		PFO("} else {\n");
		PFO("for (j%d++; j%d<", i, i);
		pa(e->LEFT->RIGHT);
		PFO("->len&&!(");
		pa(e->LEFT->RIGHT);
		PFO("->tab[j%d]); j%d++)\n", i,i);
		PFO(";\nbkt%d=(", i);
		pa(e->LEFT->RIGHT);
		PFO("->tab[j%d]);\n", i);
		PFO("if (j%d<", i);
		pa(e->LEFT->RIGHT);
		PFO("->len) {\n");
		PFO("l%d--;\n", i);
		PFO("elem%d=bkt%d;\n}\n", i, i);
		PFO("else break;\n}\n");
		if (OPTVAR(e->LEFT->LEFT->t) ||
		    e->LEFT->LEFT->t==PARAMETER ||
		    e->LEFT->LEFT->t==VARPARAM) {
			PFO("vassign_str(");
			e->LEFT->LEFT->w=YUNK;
			emit(e->LEFT->LEFT);
			PFO(", ");
			PFO("S(elem%d));\n", i);
		} else ER "No int subscripts allowed" SX;
		PFO("{\n");
		evalord(e->RIGHT);
		emit(e->RIGHT);
		droprets(e->RIGHT, 0, FALSE);
		PFO("}\nLABEL%d:", looptype);
		PFO("bkt%d=bkt%d->next", i, i);
		arrpop();
		PFO(";\n}\n}\n");
		looptype=saveloop;
		break;
	case WHILE:
		saveloop=looptype; looptype=0;
		PFO("while (");
		evalord(e->LEFT);
		femit(e->LEFT, ")\n{\n");
		droprets(e->LEFT, 0, FALSE);
		evalord(e->RIGHT);
		emit(e->RIGHT);
		droprets(e->RIGHT, 0, FALSE);
		PFO("}\n");
		droprets(e->LEFT, 0, TRUE);
		looptype=saveloop;
		break;
	case DO:
		saveloop=looptype; looptype=0;
		PFO("do {\n");
		droprets(e->narg[0], 0, TRUE); 
		evalord(e->RIGHT);
		emit(e->RIGHT);
		droprets(e->RIGHT, 0, TRUE);
		PFO("}\nwhile (");
		evalord(e->LEFT);
		femit(e->LEFT, ");\n");
		droprets(e->LEFT, 0, FALSE);
		looptype=saveloop;
		break;
	case FOR:
		saveloop=looptype; looptype=0;
		PFO("for (");
		evalord(e->narg[0]);
		femit(e->narg[0], ";");
		evalord(e->narg[1]);
		femit(e->narg[1], ";");
		evalord(e->narg[2]);
		femit(e->narg[2], ")\n{\n");
		droprets(e->narg[0], 0, TRUE);
		droprets(e->narg[1], 0, FALSE);
		droprets(e->narg[2], 0, TRUE);
		evalord(e->narg[3]);
		emit(e->narg[3]);
		droprets(e->narg[3], 0, FALSE);
		PFO("}\n");
		droprets(e->narg[0], 0, TRUE);
		droprets(e->narg[1], 0, TRUE);
		droprets(e->narg[2], 0, TRUE);
		looptype=saveloop;
		break;
	case COMPSTMT:
		for (e=e->nnext; e; e=e->nnext) {
			evalord(e);
			emit(e);
			if (!ISC1(e->t))
				PFO(";\n");
			droprets(e, 0, FALSE);
			if (e->t==RETURN || e->t==BREAK || e->t==CONTINUE)
				break;
		}
		break;
	case CLOSE:
		memit("filedrop(", e->LEFT, ");\n");
		break;
	case DELETE:
		fixdel(e->LEFT->LEFT, e->LEFT->s, 1); 
		PFO("delete(");
		emitconcat(e->LEFT->LEFT);
		PFO(",");
		pa(e->LEFT);
		PFO(");\n");
		fixdel(e->LEFT->LEFT, e->LEFT->s, 0); 
		break;
	case CONTINUE:
		if (looptype) {
			PFO("goto LABEL%d", looptype);
			break;
		}
	case BREAK:
	case NEXT:
		tokname(1,e->t);
		break;
	case EXIT:
		if (e->LEFT)
			memit("EXIT(2, ", e->LEFT, ");\n");
		PFO("if (notseenend) END();EXIT(0,0)");
		break;
	default:
		fprintf(stderr, "ERR: %d\n", e->t);
	}
}

putend(e,r,f)
int	e, r, f;
{
	char	*p;

	if (e) p=r?"s(ORS":"c('\\n'";
	else p=f?"s(OFS":"c(' '";
	PFO("fput%s, pfp)%s\n", p, e?"":";");
}
