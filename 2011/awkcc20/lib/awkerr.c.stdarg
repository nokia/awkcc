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
	
extern VARP	Au_FILENAME;
extern VARP	tmpvar;
extern int	getfnr();

#ifdef SYSV
awkerr(s)
char	*s;
{
	fprintf(stderr,  "\nawkcc: %s.\n", s);
	_awkerr();
}
#else
#if defined(__linux__) || defined(__CYGWIN__) || defined(__MACH__)
awkerr(char *format, ...)
{
	va_list	ap;

	fprintf(stderr, "\nawkcc: ");
	va_start(ap, format);
	(void)vfprintf(stderr,format, ap);
	va_end(ap);
	_awkerr();
}
#else
/* better, but nonportable (system V doesn't like doprnt) */
awkerr(char *format, ...)
{
	va_list	ap;

	fprintf(stderr, "\nawkcc: ");
	va_start(ap,format);
	(void)_doprnt(format, ap, stderr);
	va_end(ap);
	_awkerr();
}
#endif
#endif

awkperr()
{
	perror("\nawkcc");
	_awkerr();
}

_awkerr()
{
	fprintf(stderr, "input record number %d, file %s.\n", 
		getfnr(),
		TS(Au_FILENAME));
	EXIT(1, -1);
}
