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
#include "ear.h"

extern int	ND;
extern int	NF;
extern VARP	*D;
extern VARP	mD0();
extern int	pristine;
extern void	assign_NF();
extern VARP	nullfld;
extern VARP	m_var();

VARP	
getd(j, setnf)
register int	j;
int	setnf;
{
	register int	i;

	if (j>=1 && j<=NF) return(D[j]);
	if (!j) return (pristine?D[0]:mD0());
	if (j<0) ER "Illegal field $(%d)", j SX;
	if (!setnf) {
#ifdef DEBUG
		if (S(nullfld)[0]	||
		    nullfld->num	||
		    (nullfld->cur != (STRFLD|DIDFLD|NUMFLD))) {
			print_var("NLFD", nullfld);
			ER "Corrupt nullfld" SX
		}
#endif
		return (nullfld);
	}
	if (j>NF) {
		for (i=NF+1; i < ND; i++) {
			D[i]->cur=STRFLD|DIDFLD;
			if (R(D[i])) {
				R(D[i])--;
				D[i]->string=m_str(1, "");
			} else *(S(D[i]))='\0';
		}
	}
	if (j>=ND) {
		D=(VARP *)realloc((char *)D, (j+1)*sizeof(VARP));
		SP(D, "getd");
		for (i=ND; i <= j; i++) {
			D[i]=m_var();
			D[i]->cur=STRFLD|DIDFLD;
		}
		ND=1+j;
	}
	pristine=0;
	NF=j;
	assign_NF();
	return(D[j]);
}


VARP	
riggetd(v, setnf, exi)
register VARP	v;
int	setnf;
int	*exi;
{
	register int	i;

	if (!DIDCHECK(v->cur))
		v->cur|=isitanum(S(v));
	if (ISNUM(v->cur)) {
		if (!NUMAVAIL(v->cur)) {
			v->num=atof(S(v));
			v->cur|=NUMFLD;
		}
		(*exi)=i=(int) v->num;
	} else ER "Illegal field $(%s)", S(v) SX;
	return(getd(i, setnf));
}
