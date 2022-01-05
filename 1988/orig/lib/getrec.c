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

extern int	rsdflt;
extern char	*RS;

getrec(var, stream)
register VARP	var;
register FILE	*stream;
{
	register int	i;

	if (R(var)) {
		R(var)--;
		var->string=m_str(L(var), S(var));
	}
	i=L(var);
	if (i<=1) SP(S(var)=realloc(S(var), i=64), "getr1");
	if (!rsdflt) {
		S(var)[i-2]='\n';
		if (fgets(S(var), i, stream)>0) {
			while (S(var)[i-2] && S(var)[i-2]!='\n') {
				SP(S(var)=realloc(S(var),i+=64), "getr2");
				S(var)[i-2]='\n';
				fgets(S(var)+i-65, 65, stream);
			}
			L(var)=i;
			var->cur=INITSTR;
			return 1;
		} else {
			L(var)=i;
			return 0;
		}
	} else if (rsdflt==1) {
		register int	c;
		register int	j;
		int	k;
		char	*tmprs;

		tmprs=RS;
		for (j=0; (c=getc(stream))!=EOF; j++) {
			if (j>=i) 
				SP(S(var)=realloc(S(var),i+=64), "getr3");
			if (!*tmprs) {
				k=strlen(RS);
				S(var)[j-k]='\n';
				S(var)[1+j-k]='\0';
				ungetc(c, stream);
				L(var)=i;
				var->cur=INITSTR;
				return 1;
			}
			S(var)[j]=c;
			if (c==*tmprs)
				tmprs++;
			else tmprs=RS;
		}
		L(var)=i;
		return 0;
	} else {
		register int	c;
		int	j;

		S(var)[i-2]='\n';
		if ((fgets(S(var), i, stream))>0) {
			while (S(var)[i-2] && S(var)[i-2]!='\n') {
				SP(S(var)=realloc(S(var),i+=64), "getr4");
				S(var)[i-2]='\n';
				fgets(S(var)+i-65, 65, stream);
			}
			while ((c=getc(stream)) != '\n' && c!=EOF) {
				ungetc(c, stream);
				j=strlen(S(var));
				fgets(S(var)+j, i-j-1, stream);
			}
			L(var)=i;
			var->cur=INITSTR;
			return 1;
		} else {
			L(var)=i;
			return 0;
		}
	}			
}	
