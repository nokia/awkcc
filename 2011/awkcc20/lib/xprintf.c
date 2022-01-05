/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <stdio.h>
#include <stdarg.h>
#include "ear.h"

#define SRESIZE(arg)	sresize((outvar), currpos+num+arg)

#define FPRINTER(arg)	fprintf(outfile, begin, arg)

#define SPRINTER(arg)	sprintf(S((outvar))+currpos, begin, arg)

#define INCURPOS	(currpos+=strlen(S((outvar))+currpos))

#define	ERRCHECK	if (j>=nargs) {					\
				fprintf(stderr,"Control: %s.", format);	\
				awkerr("not enough args in printf");	\
			}

extern char	gnfmt();

char	*
xprintf(int act, FILE *outfile, VARP outvar, int nargs, char *format, ...)
{
	va_list	ap;
	int	j;
	char	type;
	char	*begin;
	char	*end;
	char	save;
	int	num;
	VARP	varargtmp;
	VARP	tmpvar;
	int	currpos;
	extern STRP	m_str();

	va_start(ap, format);
	currpos=0;
	if (act) {
		if (R((outvar))) {
#ifdef DEBUG
			fprintf(stderr, "[INTERNAL WARN: out in xprintf]\n");
#endif
			R((outvar))--;
			(outvar)->string=m_str(1, "");
		}
	}
	for (j=0; j<=nargs; j++) {
		varargtmp=va_arg(ap, VARP);
		FAKEOUT:;
		if (j) {
			type=gnfmt((char *)NULL, &num, &begin, &end, 1);
			begin=begin+1;
		} else type=gnfmt(format, &num, &begin, &end, 1);
		save=(*(end+1));
		*(end+1)='\0';
		switch(type) {
		case 'c':
		case 'd':
		case 'o':
		case 'x':
			ERRCHECK
			TONUM(varargtmp);
			if (act) {
				SRESIZE(CONVSIZ);
				SPRINTER((int) varargtmp->num);
				INCURPOS;
			} else {
				FPRINTER((int) varargtmp->num);
			}
			break;
		case 'e':
		case 'f':
		case 'g':
			ERRCHECK
			TONUM(varargtmp);
			if (act) {
				SRESIZE(CONVSIZ);
				SPRINTER((double) varargtmp->num);
				INCURPOS;
			} else {
				FPRINTER((double) varargtmp->num);
			}
			break;
		case 's':
			ERRCHECK
			TS(varargtmp);
			if (act) {
				SRESIZE(L(varargtmp));
				SPRINTER(S(varargtmp));
				INCURPOS;
			} else {
				FPRINTER(S(varargtmp));
			}
			break;
		case '\0':
			if (act) {
				SRESIZE(0);
				SPRINTER("");
				INCURPOS;
			} else {
				FPRINTER("");
			}
			*(end+1)=save;
			va_end(ap);
			if (act)
				return(S((outvar)));
			else {
				if (ferror(outfile))
					awkerr("failed write - file system full?");
				return (char *) NULL;
			}
		default:
			if (act)
				SPRINTER("");
			else FPRINTER("");
			*(end+1)=save;
			goto FAKEOUT;
		}
		*(end+1)=save;
	}
	va_end(ap);
	if (act)
		return(S((outvar)));
	else  {
		if (ferror(outfile))
			awkerr("failed write - file system full?");
		return (char *) 0;
	}
}

char
gnfmt(str, num, begin, end, cbegin)
char	*str;
int	*num;
char	**begin;
char	**end;
int	cbegin;
{
	static char	*internal;
	int	c;

	if (str) internal=str;
	*num=0;
	if (cbegin)
		*begin=internal;
	while ((c=(*internal)) && c!='%') {
		internal++;
		++*num;
	}
	switch(c) {
	case '%':
		switch(*(internal+1)) {
		case '%':	
			++*num;
			internal=internal+2;
			return(gnfmt((char *)NULL, num, begin, end, 0));
		}
		break;
	default:
		/*  Must have reached end of string */
		*end=internal;
		return('\0');
	}
	c=(*++internal);
	if (c=='-') {
		++*num;
		c=(*++internal);
	}
	if (c>='0' && c<='9') {
		(*num)+=atoi(internal);
		while (c>='0' && c<='9') {
			++*num;
			c=(*++internal);
		}
	}
	if (c=='.') {
		++*num;
		c=(*++internal);
	}
	if (c>='0' && c<='9') {
		(*num)+=atoi(internal);
		while (c>='0' && c<='9') {
			++*num;
			c=(*++internal);
		}
	}
	if (c=='l') c=(*++internal);
	switch(c) {
	case 'c': case 'd': case 'e': case 'f':
	case 'g': case 'o': case 'x': case 's':
		*end=internal;
		return(c);
	default:
		fprintf(stderr, "Illegal format %%%c.\n", c);
		awkerr("No such format option in [s]printf; use %% to print a % sign");
	}
	/* NOTREACHED */
}
