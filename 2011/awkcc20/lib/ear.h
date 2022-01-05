/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#define MAX_NUMBERS	299
#define MAX_NAMED	20
#define BLOCKSIZ	1024
/* PSF Changed  200 to 256 to handle non-English characters */
#define MAX_CHARS	256
#define CONVSIZ		30
#define FMTSIZ		10
#define HTBSIZ		257

#include <math.h>
#if defined(__linux__) || defined(__CYGWIN__) || defined(__MACH__) || defined(__sun__)

#include <stdlib.h>
#endif

#include <sys/param.h>
#ifdef NOFILE
#	define MAXFILES	NOFILE-5
#else
#	define MAXFILES	15
#endif

#define STRFLD		(1<<0)
#define NUMFLD		(1<<1)
#define ISFLD		(1<<2)
#define DIDFLD		(1<<3)
#define MUSTFLD		(1<<4)	/* e.g. is it truly of type string? */
#define GNUMFLD		(1<<5)
#define NEWSTR		(STRFLD|GNUMFLD)

#define STRAVAIL(x)	(x& STRFLD)
#define NUMAVAIL(x)	(x& NUMFLD)
#define ISNUM(x)	(x&  ISFLD)
#define DIDCHECK(x)	(x& DIDFLD)
#define MUSTSTR(x)	(x&MUSTFLD)
#define GNUMSTR(x)	(x&GNUMFLD)

#define INITNUM		(NUMFLD|ISFLD|DIDFLD)
#define INITSTR		(STRFLD)

#define REV	1
#define NOREV	0

#define EQ	1
#define GTE	2
#define GT	3
#define LTE	4
#define LT	5
#define NEQ	6

/* This breaks sun with gcc - is it needed anywhere?? */
#if defined(__sun__) && 0
/* These are defined in stdlib.h */
extern char	*malloc(), *realloc(), *calloc();
#endif

typedef struct {
	char	*str;		/* the actual str 		    */
	int	len;		/* len of malloc'ed space 	    */
	int	refcnt;		/* number of times this string in use */
} STR, *STRP;

typedef struct {
	STRP	string;
	double	num;	    	/* num if pss and used      	    */
	char	cur;        	/* indication of current type	    */
} VAR, *VARP;

typedef struct {
	VARP	var;
	char	*begin;
	char	*end;
	char	conv;
} DOLL, DOLLP;

#define SYSTEM(s)	(fflush(stdout), system(s))

/* SIMPLE ONLY */

#define TN(a)		(a->cur|=NUMFLD,a->num=atof(S(a)))

#define FN(a) 		(NUMAVAIL(a->cur)||TN(a))

#ifdef PC7300
#define FS(a)	 	(STRAVAIL(a->cur)?1:(ToStr(a), 1))
#define TONUM(a) 	(NUMAVAIL((tmpvar=a)->cur)?		\
				tmpvar->num:			\
				(tmpvar->cur|=NUMFLD,		\
				 tmpvar->num=atof(S(tmpvar)),	\
				 tmpvar->num))
#else 
#define FS(a)	 	(STRAVAIL(a->cur)||(ToStr(a), 1))
#define TONUM(a) 	(NUMAVAIL((tmpvar=a)->cur)?		\
				tmpvar->num:			\
				(tmpvar->cur|=NUMFLD,		\
				 tmpvar->num=atof(S(tmpvar))))
#endif

/* OK */


/* SIDE EFFECT WARNING */
#define TS(a) 		(STRAVAIL((tmpvar=a)->cur)?		\
				S(tmpvar):			\
				(ToStr(tmpvar),S(tmpvar)))

#define ITOA(a,b)	(sprintf(a, "%d", b), a)

#define FTOA(a,b)	(sprintf(a, OFMT, b), a)

#define COPYSTR(a)	(R((a))++)

#define FREESTR(a)	(free_str(a.string))

#define CMAKEARR(f,a)	if (f) { a = m_array(); }

#define CDROPARR(f,a)	if (f) { free_array(a); }

#define FREEFTM(f)	free_var(f); f=(VARP) NULL;

#define CFREEFTM(f)	if (f) { FREEFTM(f) }

#define S(a)	(a->string->str)

#define L(a)	(a->string->len)

#define R(a)	(a->string->refcnt)

#define num_preid(op, a)	(op a)

#define num_postid(op, a)	(a op)

#define VANUM(a,b)	(*assignnum(a, b))

#define ANUM(a,b)	(nassignnum(a,b))

#define VASTR(a,b)	(*vassign_str(a,b))

#define ASTR(a,b)	S(vassign_str(a,b))

#define ANAS(a,b)	(anas(a,b))

#define AVARS(a,b)	assignvars(a,b)

#define NVASTR(a,b)	(vassign_str(a,b))

#define NVANUM(a,b)	(assignnum(a, b))

#include "specassign.h"

/* Getlines
 */

#define GETL(v)		xgetl(v)
				
#define XGETR(f,n)	xfileget(f, (VARP) NULL, n)

#define XGETL(f,v,n)	xfileget(f, v, n)

#define INTMOD(a, b)	(a % b)

#define FLTMOD(a, b)	awkmod(a, b)

#define GENEQ(op, a, b) (opassign(op, a, b))

/* builtins.h
 */

#define SUBSTR(v,s,i,n,a)	(Asubstr(v,s,i,n,a))

#define CINDEX(s1,s2)	Acindex(s1,*s2)

#define HINDEX(s1,s2)	Ahindex(s1,s2)

#define INDEX(s1,s2)	Asindex(s1,s2)

#define FRAND		((double) (rand() % 32767) / 32767.0)

#define FSRAND(x)	(srand(x))

#define SUB(v,r,s,t,p)	Agsub(v,r,s,t,p,1)

#define GSUB(v,r,s,t,p)	Agsub(v,r,s,t,p,0)

#define MATCHFCN(s,r)	matchfcn(s, r)

#define MATCH(q,z)	match(z, q)

#define NOMATCH(s,r)	(!match(r,s))
#include "dollars.h"

#define ER (sprintf(errmsg,

#define SX ), awkerr(errmsg))

extern char	errmsg[];

#define SP(x, f)	((x) || ER "Out of space in %s", f SX)

#define HAVENUM(n)	((long)n==n && n>=0 && n<MAX_NUMBERS)
