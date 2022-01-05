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
#include "awk.h"

extern char	*patbeg;
extern int	patlen;
	
extern char	ctest[MAX_CHARS];
	
/* -2 means that's all we need. 
 * -1 means get everything else
 * 0 means count NF, but don't insert
 */
	
extern int	upf[];

extern int	rsdflt;	
extern int	ND;
extern int	NF;
extern int	NC;
extern int	pristine;
extern char	*STRFS;
	
extern VARP	*D;
extern fa	*REFS;

void
resplitrec(bufp, upf)
register char 	*bufp;
int	*upf;
{
	register int	lft;
	register char	*vstr;
	register VARP		tmp;
	VARP	m_var();
	int	tempstat;
	fa	*resep;

	resep=REFS;
	NF=0;
	NC=(int)bufp;
	tempstat=resep->initstat;
	for (;;) {
		if (++NF >= ND) {
			SP(D=(VARP *)realloc((char *)D, (++ND)*sizeof(VARP)), "resplitrec");
			D[NF]=m_var();
		}
		if (*upf==NF || *upf==-1) {
			upf+=(*upf>0)?1:0;
		} else if (*upf==-2) {
			bufp[(NC=strlen(bufp)-1)]='\0';
			return;
		} else {
			if (nematch(resep, bufp)) {
				resep->initstat=2;
				bufp=patbeg+patlen;
			} else {
				resep->initstat=tempstat;
				while (*bufp++)
					;
				break;
			}
			continue;
		}
		(tmp=D[NF])->cur=INITSTR;
		if (R(tmp)) {
			R(tmp)--;
			tmp->string=m_str(L(tmp), "");
		}
		vstr=S(tmp);
		lft=L(tmp)-1;
		if (nematch(resep, bufp)) {
			resep->initstat=2;
			while (bufp<patbeg) {
				(*vstr++)=(*bufp++);
				if (!lft--) {
					SP(S(tmp)=realloc(S(tmp), L(tmp)+=8), "resplitrec");
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
					SP(S(tmp)=realloc(S(tmp),L(tmp)+=8), "resplitrec");
					vstr=S(tmp)+L(tmp)-8;
					lft=7;
				}
			}
			*(vstr-2)='\0';
			break;
		}
	}
	*(bufp-2)='\0';
	pristine=1;
	NC=(int)bufp-NC-2;
}
