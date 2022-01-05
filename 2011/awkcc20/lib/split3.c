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
#include "hash.h"
#include "awk.h"

extern char	*patbeg;
extern int	patlen;
extern char	*numbers[];
extern STRP	m_str();

split3(bufp, array, resep)
register char 	*bufp;
HTBP	array;
fa	*resep;
{
	register int	lft;
	register char	*vstr;
	VARP		tmp, m_var(), s_insert();
	char	tmpstr[CONVSIZ];
	int	tempstat;
	int	i;
	static int	arrayupf[]={-1};	/* for now, get everything */
	int	*upf;

	upf=arrayupf;
	tempstat=resep->initstat;
	for (i=1; ; i++) {
		if (*upf==i || *upf==-1) {
			upf+=(*upf>0)?1:0;
		} else if (*upf==-2) {
			bufp[strlen(bufp)-1]='\0';
			return(0);
		} else {
			if (nematch(resep, bufp)) {
				resep->initstat=2;
				bufp=patbeg+patlen;
			} else {
				resep->initstat=tempstat;
				bufp=strlen(bufp)+bufp;
				break;
			}
			continue;
		}
		if (i<MAX_NUMBERS) {
			tmp=s_insert(numbers[i], array);
			tmp->string=m_str(1, "");
		} else {
			sprintf(tmpstr, "%d", i);
			tmp=s_insert(tmpstr, array);
			tmp->string=m_str(1, "");
		}
		tmp->cur=INITSTR;
		vstr=S(tmp);
		lft=L(tmp)-1;
		if (nematch(resep, bufp)) {
			resep->initstat=2;
			while (bufp<patbeg) {
				(*vstr++)=(*bufp++);
				if (!lft--) {
					SP(S(tmp)=realloc(S(tmp), L(tmp)+=8), "split3");
					vstr=S(tmp)+L(tmp)-8;
					lft=7;
				}
			}
			bufp=patbeg+patlen;
			*vstr='\0';
		} else {
			resep->initstat=tempstat;
			while ((*vstr++)=(*bufp++)) {
				if (!lft--) {
					S(tmp)=realloc(S(tmp),L(tmp)+=8);
					vstr=S(tmp)+L(tmp)-8;
					lft=7;
				}
			}
			*--vstr='\0';
			break;
		}
	}
	return(i);
}
