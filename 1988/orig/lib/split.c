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
#include "hash.h"
#include "awk.h"

extern char	*numbers[];
extern char	sctest[MAX_CHARS];
extern char	ctest[MAX_CHARS];
extern fa	*ins_regexp();
extern fa	*REFS;
extern char	*STRFS;	
extern int	dflt;
extern STRP	m_str();

split(bufp, array, sep)
register char 	*bufp;
HTBP	array;
char	*sep;
{
	register int	lft;
	register char	c, *vstr;
	VARP		tmp, m_var(), s_insert();
	char	*thectest, tmpstr[CONVSIZ];
	int	i, *upf, putback;
	static int	arrayupf[]={-1};	/* for now, get everything */
	fa	*tmpreg;

	clear_htb(array);
	if (!*sep && dflt==2)
		return split2(bufp, array, STRFS);
	else if (strlen(sep)>1 && !isreg(sep))
		return split2(bufp, array, sep);
	else if (!*sep && dflt==3)
		return split3(bufp, array, REFS);
	else if (strlen(sep)>1 && isreg(sep)) {
		tmpreg=ins_regexp(sep, 1);
		return split3(bufp, array, tmpreg);
	}
	/*  Only possibilities left are single character or
	 *  ordinary default.
	 */
	if (*sep==' ') {
		thectest=sctest;
		*sep='\0';
		putback=1;
	} else {
		thectest=ctest;
		putback=0;
	}
	c=(*bufp);
	if (!*sep) {
		while ((c=(*bufp++)) && thectest[c])
			;
		--bufp;
		c=(*(bufp));
	}
	upf=arrayupf;
	for (i=1; c; i++) {
		if (*upf==i || *upf==-1) {
			upf+=(*upf>0)?1:0;
		} else if (*upf==-2) {
			bufp[strlen(bufp)-1]='\0';
			if (putback) *sep=' ';
			return(0);
		} else {
			while ((c=(*bufp++)) && 
				((*sep)?(c!=*sep):!thectest[c]))
				;
			if (!*sep) {
				while ((c=(*bufp++)) && thectest[c])
					;
				bufp--;
				c=(*(bufp-1));
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
		while ((c=(*vstr++)=(*bufp++)) &&
			((*sep)?(c!=*sep):!thectest[c])) {
			if (!lft--) {
				SP(S(tmp)=realloc(S(tmp), L(tmp)+=8), "split");
				vstr=S(tmp)+L(tmp)-8;
				lft=7;
			}
		}
		if (!*sep && c) {
			while ((c=(*bufp++)) && thectest[c])
				;
			bufp--;
		}
		*--vstr='\0';
	}
	if (putback) *sep=' ';
	return(i-1);
}
