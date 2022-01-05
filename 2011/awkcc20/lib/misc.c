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

extern char	*numbers[];
extern STRP	m_str();
extern double	nassignnum();

#define AREF1(v,n)	\
	if (R(v)) {		\
		R(v)--;		\
		v->string=m_str(n,"");	\
	} else {	\
		if (L(v)<n) {	\
			free(S(v));	\
			SP(S(v)=malloc(n), "refalloc");	\
			L(v)=n;	\
		} \
	}

#define AREF2(v,c,n)	\
	if (R(v)) {		\
		R(v)--;		\
		v->string=m_str(n,c);	\
	} else {	\
		if (L(v)<n) {	\
			free(S(v));	\
			SP(S(v)=malloc(n), "refalloc");	\
			L(v)=n;	\
		} \
		strcpy(S(v),c); \
	}

void
ToStr(a)
register VARP	a;
{
	register double	n;
	extern char	*OFMT;	/* new */

#ifdef DEBUG
	if (STRAVAIL(a->cur)) {
		print_var("ToStr", a);
		ER "ToStr: Already converted" SX;
	}
#endif
	n=a->num;
	AREF1(a,CONVSIZ);
	if (HAVENUM(n)) {
		strcpy(S(a), numbers[(int)n]);
	} else sprintf(S(a), OFMT, n);
	a->cur|=NEWSTR;
}

anas(a,c)
register VARP	a;
char	*c;	/* string to assign */
{
	int	n;

	n=1+strlen(c);	
	AREF2(a,c,n);
	a->cur=INITSTR;
	a->cur|=isitanum(c);
	a->num=atof(c);
	a->cur|=NUMFLD;
}

/* 1 is decrement, 0 is increment */
char	*
postmp(decrp, a)
int	decrp;
register	VARP	a;
{
	register double	n;
	extern char	*OFMT;	/* new */

	if (!NUMAVAIL(a->cur))
		TN(a);
	n=a->num;
	if (!GNUMSTR(a->cur)) {
		AREF1(a,CONVSIZ);
		if (HAVENUM(n)) {
			strcpy(S(a), numbers[(int)n]);
		} else sprintf(S(a), OFMT, n);
	}
	a->num+=(decrp?-1:1);
	a->cur=INITNUM;
	return(S(a));
}

char	*
premp(decrp, a)
int	decrp;
register VARP	a;
{
	register double	n;
	extern char	*OFMT;	/* new */

	a->num+=(decrp?-1:1);
	n=a->num;
	AREF1(a,CONVSIZ);
	if (HAVENUM(n)) {
		strcpy(S(a), numbers[(int)n]);
	} else sprintf(S(a), OFMT, n);
	a->cur|=STRFLD;
	return(S(a));
}

double
postid(decrp, a)
int	decrp;
register VARP	a;
{
	extern VARP	tmpvar;
	TONUM(a);
	a->cur=INITNUM;
	if (decrp)
		return a->num--;
	else return a->num++;
}

double
preid(decrp, a)
int	decrp;
register VARP	a;
{
	extern VARP	tmpvar;
	TONUM(a);
	a->cur=INITNUM;
	if (decrp)
		return --a->num;
	else return ++a->num;
}

boolean_true(a)
VARP	a;
{
	extern VARP	tmpvar;
	if (STRAVAIL(a->cur) && NUMAVAIL(a->cur)) {
		if (!DIDCHECK(a->cur))
			a->cur|=isitanum(S(a));
		if (ISNUM(a->cur))
			return a->num?1:0;
		else return (*S(a))?1:0;
	} else if (STRAVAIL(a->cur)) {	/* STRAVAIL && !NUMAVAIL */
 		if (MUSTSTR(a->cur))
			return (*S(a))?1:0;
		else {
			if (!DIDCHECK(a->cur))
				a->cur|=isitanum(S(a));
			if (ISNUM(a->cur)) {
				TONUM(a);
				return a->num?1:0;
			} else return (*S(a))?1:0;
		}
	} else {	/* NUMAVAIL && !STRAVAIL */
		return a->num?1:0;
	}
}

VARP
vassign_str(a,c)
register VARP	a;
char	*c;	/* string to assign */
{
	int	n;

	n=1+strlen(c);
	AREF2(a,c,n);
	a->cur=INITSTR|MUSTFLD;
	return(a);
}

#include <stdarg.h>
#include <ctype.h>

VARP
Atocase(a, s, d)
register VARP	a;
register char	*s;
int	d;
{
	register char	*dest, c;
	int	n;

	n=1+strlen(s);	
	AREF1(a,n);
	dest=S(a);
	while (c=(*s++))
		if (!d)
			*dest++=(isupper(c)?tolower(c):c);
		else *dest++=(islower(c)?toupper(c):c);
	*dest='\0';
	a->cur=INITSTR|MUSTFLD;
	return(a);
}

VARP
cat(VARP a, int nargs, ...)
{
	int	n;
	register int	i;
	register char	*str;
	register char	*dest;
	va_list	ap;

	va_start(ap, nargs);
	for (i=0,n=1; i<nargs; i++) {
		n+=strlen(va_arg(ap, char *));
	}
	va_end(ap);

	AREF1(a,n);
	if (R(a)) {		
		R(a)--;		
		a->string=m_str(n,"");	
	} else {	
		if (L(a)<n) {	
			free(S(a));	
			SP(S(a)=malloc(n), "refalloc");
			L(a)=n;
		}
	}
	dest=S(a);
	va_start(ap,nargs);
	for (i=0; i<nargs; i++) {
		str=va_arg(ap, char *);
		while ((*dest++)=(*str++))
			;
		dest--;
	}
	a->cur=INITSTR|MUSTFLD;
	va_end(ap);
	return(a);
}

Acindex(str, ch)
char	*str;
register char	ch;
{
	register char	*s1;

	s1=str;
	for (; *s1 && *s1!=ch; s1++)
		;
	return (*s1?1+s1-str:0);
}

Ahindex(s, t)
char	s[];
register char	t[];
{
	register char	*s1, *s2;

	s1=s;
	s2=t;
	for (;*s2 && *s1++==*s2; s2++)
		;
	return (*s2?0:1);
}

Asindex(s, t)
char	*s;
register char	*t;
{
	register char	*s1, *s2, *s3;

	if (!*t) return 1;
	s1=s;
	while (*s1 && *s1!=*t)
		s1++;
	while (*s1) {
		while (*s1 && *s1!=*t)
			s1++;
		if (!*s1) break;
		s3=s1++;
		s2=t;
		for (;*s2 && *s3++==*s2; s2++)
			;
		if (!*s2)
			return (s1-s);
	}
	return (0);
}

copystr(v)
VARP	v;
{
	R(v)++;
}

fcopystr(v)
VARP	v;
{
	STRP	s;

	s=m_str(L(v), "");
	free_str(v->string);
	v->string=s;
}

char	*
Asubstr(v, s, i, n, all)
register VARP	v;
char	*s;
int	i;
int	n;
int	all;
{
	int	j;
	int	amt;

	j=strlen(s);
	if (i<=0) i=1;

	if (R(v)) {
		R(v)--;
		amt=j-i+2;
		if (amt<1) amt=1;
		v->string=m_str(amt, "");
	}
	*(S(v))='\0';
	if (j<i || (n<=0 && !all))
		return(S(v));
	if (all) {
		if (L(v)<j-i+2)
			SP(S(v)=realloc(S(v), j-i+2), "Asub");
		strcpy(S(v), ((i>0)?(s+i-1):s));
		return(S(v));
	}
	if (L(v) < ++n) 
		SP(S(v)=realloc(S(v), n), "Asub");
	strncpy(S(v), s+i-1, n-1);
	S(v)[n-1]='\0';
	return(S(v));
}

double
awkmod(a, b)
double	a;
double	b;
{
	double	c;
	extern double	modf();

	modf(a/b, &c);
	return (a - b * (int)c);
}

isreg(s)
register char	*s;
{
	while (*s) {
		switch(*s++) {
		case '*':
		case '+':
		case '?':
		case '.':
		case '[':
		case ']':
		case '^':
		case '(':
		case ')':
		case '$':
		case '|':
			return 1;
		case '\\':
			if (s++)
				continue;
			return 0;
		}
	} 
	return 0;
}

fixreg(s)
register char	*s;
{
	register char	*t=s;

	for (;;) {
		switch(*s) {
		case '\0': *t='\0'; return;
		case '\\':
			switch(*(s+1)) {
			case '\\': *t++='\\'; s+=2; break;
			case 'n': *t++='\n'; s+=2; break;
			case 't': 
				*t++='\t'; s+=2; break;
			case 'f': *t++='\f'; s+=2; break;
			case 'r': *t++='\r'; s+=2; break;
			case 'b': *t++='\b'; s+=2; break;
			case '\0': *t='\0'; return;
			default: *t++= *(s+1); s+=2; break;
			}
			break;
		default: *t++=(*s++);
		}
	}
}

#include <ctype.h>

/* Stolen from awk */

#define MAXEXPON	38	/* maximum exponent for fp number */

isitanum(s)
register char *s;
{
	register d1, d2;
	int point;
	char *es;

	d1 = d2 = point = 0;
	while (*s == ' ' || *s == '\t' || *s == '\n')
		s++;
	if (*s == '\0')
		return(DIDFLD);	/* empty stuff isn't number */
	if (*s == '+' || *s == '-')
		s++;
	if (!isdigit(*s) && *s != '.')
		return(DIDFLD);
	if (isdigit(*s)) {
		do {
			d1++;
			s++;
		} while (isdigit(*s));
	}
	if(d1 >= MAXEXPON)
		return(DIDFLD);	/* too many digits to convert */
	if (*s == '.') {
		point++;
		s++;
	}
	if (isdigit(*s)) {
		d2++;
		do {
			s++;
		} while (isdigit(*s));
	}
	if (!(d1 || point && d2))
		return(DIDFLD);
	if (*s == 'e' || *s == 'E') {
		s++;
		if (*s == '+' || *s == '-')
			s++;
		if (!isdigit(*s))
			return(DIDFLD);
		es = s;
		do {
			s++;
		} while (isdigit(*s));
		if (s - es > 2)
			return(DIDFLD);
		else if (s - es == 2 && 10 * (*es-'0') + *(es+1)-'0' >= MAXEXPON)
			return(DIDFLD);
	}
	while (*s == ' ' || *s == '\t' || *s == '\n')
		s++;
	if (*s == '\0')
		return(DIDFLD|ISFLD);
	else
		return(DIDFLD);
}

#include "awk.h"

matchfcn(s, r)
char	*s;
fa	*r;
{
	extern int	patlen;
	extern char	*patbeg;
	extern VARP	Au_RLENGTH;
	extern VARP	Au_RSTART;

	if (pmatch(r, s)) {
		ANUM(Au_RLENGTH, (double)((int)patlen));
		ANUM(Au_RSTART, (double)((int)(patbeg-s+1)));
		return (int)(patbeg-s+1);
	} else {
		ANUM(Au_RLENGTH, -1.0);
		ANUM(Au_RSTART, 0.0);
		return 0;
	}
}

VARP
assignvars(a,b)
register VARP	a;
register VARP	b;
{
	if (STRAVAIL(b->cur)) {
		R(b)++;
		free_str(a->string);
		a->string=b->string;
	}
	a->cur=b->cur;
	a->num=b->num;
	return(a);
}

double
nassignnum(a,b)
register VARP	a;
register double b;
{
	a->cur=INITNUM;
	a->num=b;
	return b;
}

VARP
assignnum(a,b)
register VARP	a;
register double b;
{
	a->cur=INITNUM;
	a->num=b;
	return a;
}

double
opassign(op, a, b)
int	op;
register VARP	a;
register double	b;
{
	if (!NUMAVAIL(a->cur))
		a->num=atof(S(a));
	a->cur=INITNUM;
	switch(op) {
	case '+':
		a->num+=b;
		break;
	case '-':
		a->num-=b;
		break;
	case '%':
		a->num=awkmod(a->num, b);
		break;
	case '/':
		a->num/=b;
		break;
	case '*':
		a->num*=b;
		break;
	case '^':
		a->num=pow(a->num, b);
		break;
	default:
		awkerr("fell through switch...");
	}
	return a->num;
}

extern VARP	*D;
extern int	pristine;

double
vadnum(a,b)
register VARP	a;
register double	b;
{
	int	i;

	riggetd(a, 1, &i);
	if (i) {
		D[i]->cur=INITNUM;
		D[i]->num=b;
		pristine=0;
	} else {
		D[i]->cur=INITNUM;
		D[i]->num=b;
		ToStr(D[0]);
		ad0();
	}
	return b;
}

char	*
vadstr(a,b)
register VARP	a;
register char	*b;
{
	int	i;

	riggetd(a, 1, &i);
	if (i) {
		vassign_str(D[i], b);
		pristine=0;
	} else {
		vassign_str(D[i], b);
		ad0();
		S(D[0]);
	}
	return b;
}

VARP
vadvars(a,b)
register VARP	a;
register VARP	b;
{
	int	i;
	extern VARP	tmpvar;

	riggetd(a, 1, &i);
	if (i) {
		assignvars(D[i], b);
		pristine=0;
	} else {
		assignvars(D[i], b);
		TS(D[0]);
		ad0();
	}
	return D[i];
}

extern VARP	getd();
double
adnum(a,b)
register int	a;
register double	b;
{

	if (a) {
		getd(a, 1)->cur=INITNUM;
		D[a]->num=b;
		pristine=0;
	} else {
		D[0]->num=b;
		D[0]->cur=INITNUM;
		ToStr(D[0]);
		ad0();
	}
	return b;
}

char	*
adstr(a,b)
register int	a;
register char	*b;
{
	if (a) {
		vassign_str(getd(a,1), b);
		pristine=0;
	} else {
		vassign_str(D[0], b);
		ad0();
	}
	return S(D[a]);
}

VARP	advars(a,b)
register int	a;
register VARP	b;
{
	extern VARP	tmpvar;
	if (a) {
		assignvars(getd(a, 1), b);
		pristine=0;
	} else {
		assignvars(D[0], b);
		TS(D[0]);
		ad0();
	}
	return D[a];
}
