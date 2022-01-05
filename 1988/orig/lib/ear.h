/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/

#define MAX_NUMBERS	299
#define MAX_NAMED	20
#define BLOCKSIZ	1024
#define MAX_CHARS	200
#define CONVSIZ		30
#define FMTSIZ		10
#define HTBSIZ		257

#include <sys/param.h>
#ifdef NOFILE
#	define MAXFILES	NOFILE-5
#else
#	define MAXFILES	15
#endif

#define NEWSTR		(00041)	/*STRFLD|GNUMFLD*/
#define STRFLD		(00001)
#define NUMFLD		(00002)
#define ISFLD		(00004)
#define DIDFLD		(00010)
#define MUSTFLD		(00020)
#define GNUMFLD		(00040)

#define STRAVAIL(x)	(x& STRFLD)
#define NUMAVAIL(x)	(x& NUMFLD)
#define ISNUM(x)	(x&  ISFLD)
#define DIDCHECK(x)	(x& DIDFLD)
#define MUSTSTR(x)	(x&MUSTFLD)
#define GNUMSTR(x)	(x&GNUMFLD)

#define INITNUM		(00016)
#define INITSTR		(00001)

#define REV	1
#define NOREV	0

#define EQ	1
#define GTE	2
#define GT	3
#define LTE	4
#define LT	5
#define NEQ	6

extern char	*malloc(), *realloc(), *calloc();
extern double	atof();

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

#define IF(a)	 	((NUMAVAIL((tmpvar=a)->cur) && 		\
				(MUSTSTR(tmpvar->cur)	||	\
			 	 !STRAVAIL(tmpvar->cur)))?	\
				(tmpvar->num && 1):		\
				((tmpvar->cur|=isitanum(S(tmpvar))),\
				 ((ISNUM(tmpvar->cur) && 	\
				  !MUSTSTR(tmpvar->cur))?	\
					(TONUM(tmpvar),		\
					 (tmpvar->num && 1)):	\
					(*(S(tmpvar)) && 1))))

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

extern STRP	m_str(), m_strd();

#define XPREID(op, a)	(op a)

#define XPOSTID(op, a)	(a op)

/* TONUM sets tmpvar */
#define PREID(op, a)	(TONUM(a),				\
			 (tmpvar->cur=INITNUM),			\
			 op (tmpvar->num))

#define POSTID(op, a)	(TONUM(a),				\
			 (tmpvar->cur=INITNUM),			\
			 (tmpvar->num) op)


/* Next three used in the remainder of this section
 */

#define TEMP1(a,b)	((tmpnum=(b)), (tmpvar=(a)))

#define TEMP2(a,b)	((tmpnum=(b)), (tmpvar=(a)), (tmpvar->cur=INITNUM))

#define SUBTONUM(a)	((NUMAVAIL(a->cur) || (tmpvar->num=atof(S(a)))),\
			 (a->cur=INITNUM))


/*  This should only take place with simple vars, e.g. in parameter
 *  assignment, so no tricks are necessary.
 */
#define VANUM(a,b)	((a)->cur=INITNUM,			\
			 (a)->num=b,				\
			 *(a))

#define ANUM(a,b)	(TEMP2(a, b),				\
			 (tmpvar->num=tmpnum))

#define VASTR(a,b)	(*vassign_str(a,b))

#define ASTR(a,b)	S(vassign_str(a,b))

#define ANAS(a,b)	(anas(a,b))

#define AVARS(a,b)	assignvars(a,b)

/* For the 3b2 bug (&(x,y,*(z)) is illegal for some reason */

#define NVASTR(a,b)	(vassign_str(a,b))

#define NVANUM(a,b)	((a)->cur=INITNUM,			\
			 (a)->num=b,				\
			 (a))

#include "specassign.h"

/* Getlines
 */

#define GETL(v)		xgetl(v)
				
#define XGETR(f,n)	xfileget(f, (VARP) NULL, n)

#define XGETL(f,v,n)	xfileget(f, v, n)

/* Modulo operator
 */

#define INTMOD(a, b)	(a % b)

#define FLTMOD(a, b)	awkmod(a, b)

#define CGENEQ(op, a, b) (TEMP1(a,b),				\
			  SUBTONUM(tmpvar),			\
			  (tmpvar->num op tmpnum))

#define GENEQ(op, a, b)	(TEMP2(a,b),				\
			 (tmpvar->num op tmpnum))

#define CMODEQ(a, b)	(TEMP1(a,b),				\
			 SUBTONUM(tmpvar),			\
			 (tmpvar->num=awkmod(tmpvar->num,tmpnum)))

#define MODEQ(a, b)	(TEMP2(a,b),				\
			 (tmpvar->num=awkmod(tmpvar->num,tmpnum)))

#define CPOWEQ(a, b)	(TEMP1(a,b),				\
			 SUBTONUM(tmpvar),			\
			 (tmpvar->num=pow(tmpvar->num, tmpnum)))

#define POWEQ(a, b)	(TEMP2(a,b),				\
			 (tmpvar->num=pow(tmpvar->num, tmpnum)))


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

#define MATCHFCN(s,r)	(pmatch(r, tmpchar=s)? 				  \
			  (ANUM(Au_RLENGTH, 			  \
				    (double)((int)patlen)),		  \
			   (int)ANUM(Au_RSTART, 			  \
				    (double)((int)(patbeg+1-tmpchar)))):  \
			 (ANUM(Au_RLENGTH, -1.0),			  \
			  ANUM(Au_RSTART, 0.0),			  \
			  0))
				

#define MATCH(q,z)	match(z, q)

#define NOMATCH(s,r)	(!match(r,s))
#include "dollars.h"

#define ER (sprintf(errmsg,

#define SX ), awkerr(errmsg))

extern char	errmsg[];

#define SP(x, f)	((x) || ER "Out of space in %s", f SX)

#define HAVENUM(n)	((long)n==n && n>=0 && n<MAX_NUMBERS)

#define STRCPY(a,b)	{ register char	*s1, *s2;	\
			s1=a; s2=b;			\
			while (*s1++ = *s2++) ; }

#define STRCPY2(n,a,b)	strcpy(a,b);

/*
#define STRCPY2(n,a,b)	{ if (n>9) strcpy(a,b); else STRCPY(a,b) }
*/

