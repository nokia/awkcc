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
	
extern char	*OFS;
extern int	pristine;
extern int	NF;
extern VARP	*D;

printD0(file)
FILE	*file;
{
	register int	i;
	
	if (pristine)  {
		fputs(S(D[0]), file);
		return;
	}
	if (NF<=0) return;
	for (i=1; i<NF; i++) {
		if (!STRAVAIL(D[i]->cur))
			ToStr(D[i]);
		fputs(S(D[i]), file);
		fputs(OFS, file);
	}
	if (!STRAVAIL(D[i]->cur))
		ToStr(D[i]);
	fputs(S(D[i]), file);
}

VARP
mD0()
{
	register VARP	curd;
	int	i;
	register int	j;
	register char	*str0ptr;
	register int	k;
	register int	m;
	int	curlen;

	if (pristine) 
		return(D[0]);
	if (R(D[0])) {
		R(D[0])--;
		D[0]->string=m_str(L(D[0]), S(D[0]));
	}
	k=strlen(OFS);
	str0ptr=S(D[0]);
	*str0ptr='\0';
	m=0;
	for (i=1; i<NF; i++) {
		curd=(D[i]);
		if (!STRAVAIL(curd->cur))
			ToStr(curd);
		curlen=strlen(S(curd));
		j=k+curlen+1;
		if (L(D[0])<m+j) {
			str0ptr=S(D[0])=realloc(str0ptr, L(D[0])=m+j);
		}
		STRCPY2(curlen,str0ptr+m,S(curd));
		m+=curlen;
		STRCPY(str0ptr+m,OFS);
		m+=k;
	}
	curd=(D[i]);
	if (!STRAVAIL(curd->cur))
		ToStr(curd);
	curlen=strlen(S(curd));
	j=curlen+1;
	if (L(D[0])<m+j) {
		str0ptr=S(D[0])=realloc(str0ptr, L(D[0])=m+j);
	}
	STRCPY2(curlen,str0ptr+m,S(curd));
	pristine=1;
	return(D[0]);
}
