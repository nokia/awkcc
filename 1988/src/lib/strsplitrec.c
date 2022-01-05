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
	
extern char	ctest[MAX_CHARS];
	
/* -2 means that's all we need. 
 * -1 means get everything else
 * 0 means count NF, but don't insert
 */
	
extern int	upf[];
extern int	dflt;
extern int	rsdflt;
	
extern int	ND;
extern int	NF;
extern int	NC;
extern int	pristine;
extern char	*STRFS;
	
extern VARP	*D;

void
strsplitrec(bufp, upf)
register char 	*bufp;
int	*upf;
{
	register int	lft;
	register char	c;
	register char	*vstr;
	VARP		tmp, m_var();
	char	IFS;

	IFS=(*STRFS);
	NF=0;
	NC=(int)bufp;
	c=(*bufp);
	while (c!='\n') {
	NOTREALLYDONE:;
		if (++NF >= ND) {
			D=(VARP *)realloc((char *)D, (++ND)*sizeof(VARP));
			SP(D, "strsplitrec");
			D[NF]=m_var();
		}
		if (*upf==NF || *upf==-1) {
			upf+=(*upf>0)?1:0;
		} else if (*upf==-2) {
			bufp[(NC=strlen(bufp)-1)]='\0';
			return;
		} else {
			for (;;) {
				if ((c=(*bufp++))=='\n' || !c)
					goto DONE1;
				/* STRFS points to cdr of field sep */
				if (c==IFS) {
					register char	*s1=bufp;
					register char	*s2=STRFS+1;
				
					for (;*s2 && *s1++==*s2; s2++)
						;
					if (!*s2) {
						bufp=s1;
						goto DONE1;
					}
				}
			}
			DONE1:;
			continue;
		}
		(tmp=D[NF])->cur=INITSTR;
		if (R(tmp)) {
			R(tmp)--;
			tmp->string=m_str(L(tmp), "");
		}
		vstr=S(tmp);
		lft=L(tmp)-1;
		for (;;) {
			if ((c=(*vstr++)=(*bufp++)) == '\n' || !c)
				goto DONE2;
			if (c==IFS) {
				register char	*s1=bufp;
				register char	*s2=STRFS+1;
			
				for (;*s2 && *s1++==*s2; s2++)
					;
				if (!*s2) {
					bufp=s1;
					goto DONE2;
				}
			}
			if (!lft--) {
				SP(S(tmp)=realloc(S(tmp),L(tmp)+=8), "str");
				vstr=S(tmp)+L(tmp)-8;
				lft=7;
			}
		}
		DONE2:;
		*--vstr='\0';
	}
	if (rsdflt && *bufp)
		goto NOTREALLYDONE;
	*--bufp='\0';
	pristine=1;
	NC=(int)bufp-NC;
}
