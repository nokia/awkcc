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

extern int	illum;
extern VARINFOP	*symtab;

char	*
epvar(e)
Nodep	e;
{
	static	char	p[30];

	if (ILLUM(e->s) || e->t==SYSVAR)
		return((char *)e->s);
	else {
		sprintf(p, "%d", e->sympos);
		return p;
	}
}

char	*
npvar(n)
int	n;
{
	static	char	p[30];


	if (ILLUM(symtab[n]->string) || symtab[n]->type==SYSVAR)
		return((char *)symtab[n]->string);
	else {
		sprintf(p, "%d", n);
		return p;
	}
}

tokname(inst, op)
int	inst;
int	op;
{
	char	*p;

	switch(op) {
	case SUBEQ: 
		switch(inst) {
			case 1: p="-="; break;
			case 2: p="GENEQ('-',"; break;
			default: p="PRINTOP ERROR";
		}
		break;
	case DIVEQ: 
		switch(inst) {
			case 1: p="/="; break;
			case 2: p="GENEQ('/',"; break;
			default: p="PRINTOP ERROR";
		}
		break;
	case MULTEQ: 
		switch(inst) {
			case 1: p="*="; break;
			case 2: p="GENEQ('*',"; break;
			default: p="PRINTOP ERROR";
		}
		break;
	case ADDEQ: 
		switch(inst) {
			case 1: p="+="; break;
			case 2: p="GENEQ('+',"; break;
			default: p="PRINTOP ERROR";
		}
		break;
	case MODEQ: 
		switch(inst) {
			case 1: p="%="; break;
			case 2: p="GENEQ('%%',"; break;
			default: p="PRINTOP ERROR";
		}
		break;
	case POWEQ:
		switch(inst) {
			case 1: p="GENEQ('^',"; break;
			case 2: p="GENEQ('^',"; break;
			default: p="PRINTOP ERROR";
		}
		break;
	case PREINCR:
	case PREDECR:
		{	int	decrp;
		decrp=(op==PREDECR)?1:0;
		switch(inst) {
		case 1: p=decrp?"--":"++"; break;
		case 2: p=decrp?"preid(1,":"preid(0,"; break;
		case 3: p=decrp?"premp(1,":"premp(0,"; break;
		case 5: p=decrp?"preid(--,":"preid(++,"; break;
		default: p="PRINTOP ERROR";
		}
		}
		break;
	case POSTDECR:
	case POSTINCR:
		{	int	decrp;
		decrp=(op==POSTDECR)?1:0;
		switch(inst) {
		case 1: p=decrp?"--":"++"; break;
		case 2: p=decrp?"postid(1,":"postid(0,"; break;
		case 3: p=decrp?"postmp(1,":"postmp(0,"; break;
		case 4: p=decrp?"insert(2,":"insert(1,"; break;
		case 5: p=decrp?"postid(--,":"postid(++,"; break;
		default: p="PRINTOP ERROR";
		}
		}
		break;
	case EXTERN: p=""; break;
	case FLOATVAR: p="Af_"; break;
	case INTVAR: p="Ai_"; break;
	case NEXT: p="return"; break;
	case BREAK: p="break"; break;
	case CONTINUE: p="continue"; break;
	case GE: p=inst?">=":"GTE"; break;
	case GT: p=inst?">":"GT"; break;
	case LT: p=inst?"<":"LT"; break;
	case LE: p=inst?"<=":"LTE"; break;
	case EQ: p=inst?"==":"EQ"; break;
	case NE: p=inst?"!=":"NEQ"; break;
	case UMINUS: p="-"; break;
	case NOT: p="!"; break;
	case AND: p="&&"; break;
	case BOR: p="||"; break;
	case ADD: p="+"; break;
	case MINUS: p="-"; break;
	case DIVIDE: p="/"; break;
	case MOD: case POWER: p=", "; break;
	case MULT: p="*"; break;
	case FINT: p="((int)"; break;
	case SPLIT: p="split("; break;
	case FCOS: p="cos("; break;
	case FEXP: p="exp("; break;
	case FLOG: p="log("; break;
	case FSIN: p="sin("; break;
	case FSQRT: p="sqrt("; break;
	case FSRAND: p="FSRAND("; break;
	case FLENGTH: p="strlen("; break;
	case FSYSTEM: p="SYSTEM("; break;
	case INTEST: p="member("; break;
	default: fprintf(stderr, "Unprintable operator %d(%d).\n", op, inst);
	}
	PFO(p);
}
