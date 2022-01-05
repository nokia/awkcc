%Start A str sc reg comment

%{
#include	"structs.h"
#include	"parse.h"
#include	"tokens.h"
#include	"y.tab.h"
#include	"funcs.h"

#define ERROR	ER
#define SYNTAX	SX

#undef	input	/* defeat lex */
#undef	unput

extern int	infunc;
extern int	dbg;
extern uchar	*curfname;

int	lineno	= 1;
int	bracecnt = 0;
int	brackcnt  = 0;
int	parencnt = 0;
#define DEBUG
#ifdef	DEBUG
#	define	RET(x)	{if(dbg)fprintf(stderr, "lex %d [%s]\n", x, yytext); return(x); }
#else
#	define	RET(x)	return(x)
#endif
#define	CADD	cbuf[clen++]=yytext[0]; \
		if (clen>=CBUFLEN-1) { \
			ERROR "string/reg expr %.10s... too long", cbuf SYNTAX; \
			BEGIN A; \
		}

#define	CBUFLEN	400
uchar	cbuf[CBUFLEN]; 
uchar	*s;
int	clen, cflag;
%}

A	[a-zA-Z_]
B	[a-zA-Z0-9_]
D	[0-9]
WS	[ \t]

%%
	switch (yybgin-yysvec-1) {	/* witchcraft */
	case 0:
		BEGIN A;
		break;
	case sc:
		BEGIN A;
		RET('}');
	}

<A>\n		{ lineno++; RET(NL); }
<A>#CC.*	{ lineno++; 
		  yylval.cp = xmakenode(CLINE, yytext+3, 0.0); 
		  RET(CLINE);
		}
<A>#awkcchint.*	{ lineno++;
		  hints(yytext+10);
		  RET(NL);
		}
<A>#.*		{ lineno++; RET(NL); }	/* strip comment lines */
<A>{WS}+	{ ; }
<A>;		{ RET(';'); }

<A>"\\"\n	{ lineno++; }
<A>BEGIN	{ RET(XBEGIN); }
<A>END		{ RET(XEND); }
<A>func(tion)?	{ if (infunc) ERROR "illegal nested function" SYNTAX; RET(FUNC); }
<A>return	{ if (!infunc) ERROR "return not in function" SYNTAX; RET(RETURN); }
<A>"&&"		{ RET(AND); }
<A>"||"		{ RET(BOR); }
<A>"!"		{ RET(NOT); }
<A>"!="		{ yylval.i = NE; RET(NE); }
<A>"~"		{ yylval.i = MATCH; RET(MATCHOP); }
<A>"!~"		{ yylval.i = NOTMATCH; RET(MATCHOP); }
<A>"<"		{ yylval.i = LT; RET(LT); }
<A>"<="		{ yylval.i = LE; RET(LE); }
<A>"=="		{ yylval.i = EQ; RET(EQ); }
<A>">="		{ yylval.i = GE; RET(GE); }
<A>">"		{ yylval.i = GT; RET(GT); }
<A>">>"		{ yylval.i = APPEND; RET(APPEND); }
<A>"++"		{ yylval.i = INCR; RET(INCR); }
<A>"--"		{ yylval.i = DECR; RET(DECR); }
<A>"+="		{ yylval.i = ADDEQ; RET(ASGNOP); }
<A>"-="		{ yylval.i = SUBEQ; RET(ASGNOP); }
<A>"*="		{ yylval.i = MULTEQ; RET(ASGNOP); }
<A>"/="		{ yylval.i = DIVEQ; RET(ASGNOP); }
<A>"%="		{ yylval.i = MODEQ; RET(ASGNOP); }
<A>"^="		{ yylval.i = POWEQ; RET(ASGNOP); }
<A>"**="	{ yylval.i = POWEQ; RET(ASGNOP); }
<A>"="		{ yylval.i = ASSIGN; RET(ASGNOP); }
<A>"**"		{ RET(POWER); }
<A>"^"		{ RET(POWER); }

<A>"$"{D}+	{ unputstr(yytext+1); RET(INDIRECT); }
<A>"$"{A}{B}*	{
			int c, n, tmp;
			c = input(); unput(c);
			if (c == '(' || c == '[' || infunc && (n=isarg(yytext+1)) >= 0) {
				unputstr(yytext+1);
			} else {
				yytext[0]='(';
				tmp=strlen(yytext);
				yytext[tmp+1]='\0';
				yytext[tmp]=')';
				unputstr(yytext); 
			}
			RET(INDIRECT); 
		}
<A>"$"		{ RET(INDIRECT); }
<A>"NF"		{ yylval.cp = xmakenode(SYM, yytext, 0.0); RET(VAR); }

<A>({D}+("."?){D}*|"."{D}+)((e|E)("+"|-)?{D}+)?	{
		  yylval.cp=xmakenode(NUMBER,yytext,atof(yytext));
		  RET(NUMBER); }

<A>while	{ RET(WHILE); }
<A>for		{ RET(FOR); }
<A>do		{ RET(DO); }
<A>if		{ RET(IF); }
<A>else		{ RET(ELSE); }
<A>next		{ RET(NEXT); }
<A>exit		{ RET(EXIT); }
<A>break	{ RET(BREAK); }
<A>continue	{ RET(CONTINUE); }
<A>print	{ yylval.i = PRINT; RET(PRINT); }
<A>printf	{ yylval.i = PRINTF; RET(PRINTF); }
<A>sprintf	{ yylval.i = SPRINTF; RET(SPRINTF); }
<A>split	{ yylval.i = SPLIT; RET(SPLIT); }
<A>substr	{ RET(SUBSTR); }
<A>sub		{ yylval.i = SUB; RET(SUB); }
<A>gsub		{ yylval.i = GSUB; RET(GSUB); }
<A>index	{ RET(INDEX); }
<A>match	{ RET(MATCHFCN); }
<A>in		{ RET(IN); }
<A>getline	{ RET(GETLINE); }
<A>close	{ RET(CLOSE); }
<A>delete	{ RET(DELETE); }
<A>length	{ yylval.i = FLENGTH; RET(BLTIN); }
<A>toupper	{ yylval.i = FTOUPPER; RET(BLTIN); }
<A>tolower	{ yylval.i = FTOLOWER; RET(BLTIN); }
<A>log		{ yylval.i = FLOG; RET(BLTIN); }
<A>int		{ yylval.i = FINT; RET(BLTIN); }
<A>exp		{ yylval.i = FEXP; RET(BLTIN); }
<A>sqrt		{ yylval.i = FSQRT; RET(BLTIN); }
<A>sin		{ yylval.i = FSIN; RET(BLTIN); }
<A>cos		{ yylval.i = FCOS; RET(BLTIN); }
<A>atan2	{ yylval.i = FATAN; RET(BLTIN); }
<A>system	{ yylval.i = FSYSTEM; RET(BLTIN); }
<A>rand		{ yylval.i = FRAND; RET(BLTIN); }
<A>srand	{ yylval.i = FSRAND; RET(BLTIN); }

<A>{A}{B}*	{ int n, c;
		  c = input(); unput(c);	/* look for '(' */
		  if (c != '(' && infunc && (n=isarg(yytext)) >= 0) {
			yylval.i = n;
			RET(ARG);
		  } else {
			if (c == '(') {
				yylval.cp=xmakenode(CALL, yytext, 0.0);
				RET(CALL);
			} else {
				yylval.cp=xmakenode(SYM, yytext, 0.0);
				RET(VAR);
			}
		  }
		}
<A>\"		{ BEGIN str; clen = 0; }

<A>"}"		{ if (--bracecnt < 0) yyerror("extra }"); BEGIN sc; RET(';'); }
<A>"]"		{ if (--brackcnt < 0) yyerror("extra ]"); RET(']'); }
<A>")"		{ if (--parencnt < 0) yyerror("extra )"); RET(')'); }

<A>.		{ if (yytext[0] == '{') bracecnt++;
		  else if (yytext[0] == '[') brackcnt++;
		  else if (yytext[0] == '(') parencnt++;
		  RET(yylval.i = yytext[0]); /* everything else */ }

<reg>\\.	{ cbuf[clen++] = '\\'; cbuf[clen++] = yytext[1]; }
<reg>\n		{ yyerror("newline in regular expression %.10s...", cbuf); lineno++; BEGIN A; }
<reg>"/"	{ BEGIN A;
		  cbuf[clen] = 0;
		  yylval.s = (uchar *) tostring(cbuf);
		  unput('/');
		  RET(REGEXPR); }
<reg>.		{ CADD; }

<str>\"		{ BEGIN A;
		  cbuf[clen] = 0; s = (uchar *) tostring(cbuf);
		  /* cbuf[clen] = ' '*/ ; cbuf[++clen] = 0;
		  yylval.cp = xmakenode(STRING, cbuf, 0.0);
		  RET(STRING); }
<str>\n		{ yyerror("newline in string %.10s...", cbuf); lineno++; BEGIN A; }
<str>"\\\""	{ cbuf[clen++] = '"'; }
<str>"\\"n	{ cbuf[clen++] = '\n'; }
<str>"\\"t	{ cbuf[clen++] = '\t'; }
<str>"\\"f	{ cbuf[clen++] = '\f'; }
<str>"\\"r	{ cbuf[clen++] = '\r'; }
<str>"\\"b	{ cbuf[clen++] = '\b'; }
<str>"\\\\"	{ cbuf[clen++] = '\\'; }
<str>"\\"({D}{D}{D}|{D}{D}|{D}) { int n;
		  sscanf(yytext+1, "%o", &n); cbuf[clen++] = n; }
<str>"\\".	{ cbuf[clen++] = yytext[1]; }
<str>.		{ CADD; }

%%

startreg()
{
	BEGIN reg;
	clen = 0;
}

/* input() and unput() are transcriptions of the standard lex
   macros for input and output with additions for error message
   printing.  God help us all if someone changes how lex works.
*/

uchar	ebuf[300];
uchar	*ep = ebuf;

input()
{
	register c;
	extern uchar *lexprog;

	if (yysptr > yysbuf)
		c = U(*--yysptr);
	else if (yyin == NULL)
		c = *lexprog++;
	else
		c = getc(yyin);
	if (c == '\n')
		yylineno++;
	else if (c == EOF)
		c = 0;
	if (ep >= ebuf + sizeof ebuf)
		ep = ebuf;
	return *ep++ = c;
}

unput(c)
{
	yytchar = c;
	if (yytchar == '\n')
		yylineno--;
	*yysptr++ = yytchar;
	if (--ep < ebuf)
		ep = ebuf + sizeof(ebuf) - 1;
}


unputstr(s)
	uchar *s;
{
	int i;

	for (i = strlen(s)-1; i >= 0; i--)
		unput(s[i]);
}
