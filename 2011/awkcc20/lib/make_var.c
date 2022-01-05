/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <stdio.h>
#include "ear.h"

#define MAX_SAVED 100

extern STRP	nullstring;

VARP
m_evar()
{
	register VARP	a;

	SP(a=(VARP)malloc(sizeof(VAR)), "m_evar");
	a->num=0.0;
	a->string=nullstring;
	nullstring->refcnt++;
	a->cur=INITNUM|INITSTR;
	return(a);
}

VARP
m_var()
{
	register VARP	a;
	extern STRP	m_str();

	SP(a=(VARP)malloc(sizeof(VAR)), "m_var");
	a->num=0.0;
	a->string=m_str(1, "");
	a->cur=INITNUM|INITSTR;
	return(a);
}

free_var(a)
register VARP	a;
{
	free_str(a->string);
	free(a);
}

print_var(str, var)
char	*str;
VARP	var;
{
	fprintf(stderr, "%s(%d,", str, var);
	fprintf(stderr, "%d,", var->string);
	fprintf(stderr, "%d): ", S(var));
	fprintf(stderr, "N:%d, ", R(var));
	fprintf(stderr, "s:%s, ", S(var));
	fprintf(stderr, "n:%.6g, ", var->num);
	fprintf(stderr, "curr:%o, ", (char)var->cur);
	fprintf(stderr, "len:%d.\n", L(var));
}
