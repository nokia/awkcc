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
#include "awk.h"
#include "ear.h"
#include "hash.h"
	
HTBP	REGEXPS1;
HTBP	REGEXPS;
fa	**regs;
int	numregs;
extern VARP	member();
extern VARP	s_insert();
extern char	errmsg[];

fa	*
ins_regexp(r, mode)
char	*r;
int	mode;
{
	VARP	tmp;
	int	i;

	if (mode) {
		if (tmp=member(r, REGEXPS1)) 
			return(regs[(int)tmp->num]);
	} else {
		if (tmp=member(r, REGEXPS)) 
			return(regs[(int)tmp->num]);
	}
	tmp=s_insert(r, (mode?REGEXPS1:REGEXPS));
	i=find_slot();
	if (!(regs[i]=mdfa(reparse(r), mode))) {
		sprintf(errmsg, "Cannot compile regexp %s.\n", r);
		awkerr(errmsg);
	}
	tmp->num=(double) i;
	return(regs[i]);
}

elim_regexp(r, mode)
char	*r;
int	mode;
{
	VARP	tmp;
	
	if (tmp=member(r, (mode?REGEXPS1:REGEXPS))) {
		free((char *)regs[(int)tmp->num]);
		regs[(int)tmp->num]=(fa *) NULL;
		delete(r, (mode?REGEXPS:REGEXPS));
		return(1);
	}
	return(0);
}

find_slot()
{
	register int	i;

	for (i=0; i<numregs && regs[i]; i++)
		;
	if (i>=numregs) {
		regs=(fa **)realloc((char *)regs, (numregs+8)*(sizeof(fa *)));
		numregs+=8;
		return(numregs-8);
	}
	return i;
}
