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

rgetrec(var)
register VARP	var;
{
	extern int	FNR;	
	extern FILE	*in;

	/* PSF 08/31/05 - make sure we don't keep reading after the file
         * is closed
	 */
	if ( in == NULL ) return(0);

	switch(getrec(var, in)) {
	case 0:
		if (opennext(1)) {
			resetfnr();
			FNR=0;
			return rgetrec(var);
		} else return 0;
		break;
	default:
		FNR++;
		return(1);
	}
	/* NOTREACHED */
	return(0);
}

getrec(var, stream)
register VARP	var;
register FILE	*stream;
{
	extern int	rsdflt;
	extern char	*RS;
	extern STRP	m_str();

	register int	i;

	if (R(var)) {
		R(var)--;
		var->string=m_str(L(var), S(var));
	}
	i=L(var);
	if (i<=1) SP(S(var)=realloc(S(var), i=64), "getr1");
	L(var)=i;
	if (!rsdflt) {
		S(var)[i-2]='\n';
		if (fgets(S(var), i, stream)) {
			while (S(var)[i-2] && S(var)[i-2]!='\n') {
				SP(S(var)=realloc(S(var),i+=64), "getr2");
				S(var)[i-2]='\n';
				if (!fgets(S(var)+i-65, 65, stream))
					break;
			}
			L(var)=i;
			var->cur=INITSTR;
			return 1;
		} else return 0;
	} else if (rsdflt==1) {
		register int	c, j;
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
		/* success anyway, just to flush input, I guess */
		S(var)[j]='\n';
		S(var)[1+j]='\0';
		ungetc(c, stream);
		L(var)=i;
		var->cur=INITSTR;
		return (j>0)?1:0;
	} else {
		register int	c, j;
		int	k;
		char	*tmprs;

		tmprs="\n\n";
		for (j=0; (c=getc(stream))!=EOF; j++) {
			if (j>=i) 
				SP(S(var)=realloc(S(var),i+=64), "getr3");
			if (!*tmprs) {
				k=strlen(RS);
				S(var)[j-k-1]='\0';
				ungetc(c, stream);
				L(var)=i;
				var->cur=INITSTR;
				return 1;
			}
			S(var)[j]=c;
			if (c==*tmprs)
				tmprs++;
			else tmprs="\n\n";
		}
		S(var)[j]='\0';
		ungetc(c, stream);
		L(var)=i;
		var->cur=INITSTR;
		return (j>0)?1:0;
	}
}
