#include <stdio.h>
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
#ifndef __cplusplus
# define output(c) (void)putc(c,yyout)
#else
# define lex_output(c) (void)putc(c,yyout)
#endif

#if defined(__cplusplus) || defined(__STDC__)

#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
	int yyback(int *, int);
	int yyinput(void);
	int yylook(void);
	void yyoutput(int);
	int yyracc(int);
	int yyreject(void);
	void yyunput(int);
	int yylex(void);
#ifdef YYLEX_E
	void yywoutput(wchar_t);
	wchar_t yywinput(void);
#endif
#ifndef yyless
	int yyless(int);
#endif
#ifndef yywrap
	int yywrap(void);
#endif
#ifdef LEXDEBUG
	void allprint(char);
	void sprint(char *);
#endif
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
	void exit(int);
#ifdef __cplusplus
}
#endif

#endif
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
#ifndef __cplusplus
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#else
# define lex_input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#endif
#define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng;
char yytext[YYLMAX];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin,*yyout;
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
# define A 2
# define str 4
# define sc 6
# define reg 8
# define comment 10
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
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;

# line 48 "awk.lx.l"
switch (yybgin-yysvec-1) {	/* witchcraft */
	case 0:
		BEGIN A;
		break;
	case sc:
		BEGIN A;
		RET('}');
	}
#ifdef __cplusplus
/* to avoid CC and lint complaining yyfussy not being used ...*/
static int __lex_hack = 0;
if (__lex_hack) goto yyfussy;
#endif
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:

# line 57 "awk.lx.l"
	{ lineno++; RET(NL); }
break;
case 2:

# line 58 "awk.lx.l"
{ lineno++; 
		  yylval.cp = xmakenode(CLINE, yytext+3, 0.0); 
		  RET(CLINE);
		}
break;
case 3:

# line 62 "awk.lx.l"
{ lineno++;
		  hints(yytext+10);
		  RET(NL);
		}
break;
case 4:

# line 66 "awk.lx.l"
	{ lineno++; RET(NL); }
break;
case 5:

# line 67 "awk.lx.l"
{ ; }
break;
case 6:

# line 68 "awk.lx.l"
	{ RET(';'); }
break;
case 7:

# line 70 "awk.lx.l"
{ lineno++; }
break;
case 8:

# line 71 "awk.lx.l"
{ RET(XBEGIN); }
break;
case 9:

# line 72 "awk.lx.l"
	{ RET(XEND); }
break;
case 10:

# line 73 "awk.lx.l"
{ if (infunc) ERROR "illegal nested function" SYNTAX; RET(FUNC); }
break;
case 11:

# line 74 "awk.lx.l"
{ if (!infunc) ERROR "return not in function" SYNTAX; RET(RETURN); }
break;
case 12:

# line 75 "awk.lx.l"
	{ RET(AND); }
break;
case 13:

# line 76 "awk.lx.l"
	{ RET(BOR); }
break;
case 14:

# line 77 "awk.lx.l"
	{ RET(NOT); }
break;
case 15:

# line 78 "awk.lx.l"
	{ yylval.i = NE; RET(NE); }
break;
case 16:

# line 79 "awk.lx.l"
	{ yylval.i = MATCH; RET(MATCHOP); }
break;
case 17:

# line 80 "awk.lx.l"
	{ yylval.i = NOTMATCH; RET(MATCHOP); }
break;
case 18:

# line 81 "awk.lx.l"
	{ yylval.i = LT; RET(LT); }
break;
case 19:

# line 82 "awk.lx.l"
	{ yylval.i = LE; RET(LE); }
break;
case 20:

# line 83 "awk.lx.l"
	{ yylval.i = EQ; RET(EQ); }
break;
case 21:

# line 84 "awk.lx.l"
	{ yylval.i = GE; RET(GE); }
break;
case 22:

# line 85 "awk.lx.l"
	{ yylval.i = GT; RET(GT); }
break;
case 23:

# line 86 "awk.lx.l"
	{ yylval.i = APPEND; RET(APPEND); }
break;
case 24:

# line 87 "awk.lx.l"
	{ yylval.i = INCR; RET(INCR); }
break;
case 25:

# line 88 "awk.lx.l"
	{ yylval.i = DECR; RET(DECR); }
break;
case 26:

# line 89 "awk.lx.l"
	{ yylval.i = ADDEQ; RET(ASGNOP); }
break;
case 27:

# line 90 "awk.lx.l"
	{ yylval.i = SUBEQ; RET(ASGNOP); }
break;
case 28:

# line 91 "awk.lx.l"
	{ yylval.i = MULTEQ; RET(ASGNOP); }
break;
case 29:

# line 92 "awk.lx.l"
	{ yylval.i = DIVEQ; RET(ASGNOP); }
break;
case 30:

# line 93 "awk.lx.l"
	{ yylval.i = MODEQ; RET(ASGNOP); }
break;
case 31:

# line 94 "awk.lx.l"
	{ yylval.i = POWEQ; RET(ASGNOP); }
break;
case 32:

# line 95 "awk.lx.l"
{ yylval.i = POWEQ; RET(ASGNOP); }
break;
case 33:

# line 96 "awk.lx.l"
	{ yylval.i = ASSIGN; RET(ASGNOP); }
break;
case 34:

# line 97 "awk.lx.l"
	{ RET(POWER); }
break;
case 35:

# line 98 "awk.lx.l"
	{ RET(POWER); }
break;
case 36:

# line 100 "awk.lx.l"
{ unputstr(yytext+1); RET(INDIRECT); }
break;
case 37:

# line 101 "awk.lx.l"
{
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
break;
case 38:

# line 115 "awk.lx.l"
	{ RET(INDIRECT); }
break;
case 39:

# line 116 "awk.lx.l"
	{ yylval.cp = xmakenode(SYM, yytext, 0.0); RET(VAR); }
break;
case 40:

# line 118 "awk.lx.l"
{
		  yylval.cp=xmakenode(NUMBER,yytext,atof(yytext));
		  RET(NUMBER); }
break;
case 41:

# line 122 "awk.lx.l"
{ RET(WHILE); }
break;
case 42:

# line 123 "awk.lx.l"
	{ RET(FOR); }
break;
case 43:

# line 124 "awk.lx.l"
	{ RET(DO); }
break;
case 44:

# line 125 "awk.lx.l"
	{ RET(IF); }
break;
case 45:

# line 126 "awk.lx.l"
	{ RET(ELSE); }
break;
case 46:

# line 127 "awk.lx.l"
	{ RET(NEXT); }
break;
case 47:

# line 128 "awk.lx.l"
	{ RET(EXIT); }
break;
case 48:

# line 129 "awk.lx.l"
{ RET(BREAK); }
break;
case 49:

# line 130 "awk.lx.l"
{ RET(CONTINUE); }
break;
case 50:

# line 131 "awk.lx.l"
{ yylval.i = PRINT; RET(PRINT); }
break;
case 51:

# line 132 "awk.lx.l"
{ yylval.i = PRINTF; RET(PRINTF); }
break;
case 52:

# line 133 "awk.lx.l"
{ yylval.i = SPRINTF; RET(SPRINTF); }
break;
case 53:

# line 134 "awk.lx.l"
{ yylval.i = SPLIT; RET(SPLIT); }
break;
case 54:

# line 135 "awk.lx.l"
{ RET(SUBSTR); }
break;
case 55:

# line 136 "awk.lx.l"
	{ yylval.i = SUB; RET(SUB); }
break;
case 56:

# line 137 "awk.lx.l"
	{ yylval.i = GSUB; RET(GSUB); }
break;
case 57:

# line 138 "awk.lx.l"
{ RET(INDEX); }
break;
case 58:

# line 139 "awk.lx.l"
{ RET(MATCHFCN); }
break;
case 59:

# line 140 "awk.lx.l"
	{ RET(IN); }
break;
case 60:

# line 141 "awk.lx.l"
{ RET(GETLINE); }
break;
case 61:

# line 142 "awk.lx.l"
{ RET(CLOSE); }
break;
case 62:

# line 143 "awk.lx.l"
{ RET(DELETE); }
break;
case 63:

# line 144 "awk.lx.l"
{ yylval.i = FLENGTH; RET(BLTIN); }
break;
case 64:

# line 145 "awk.lx.l"
{ yylval.i = FTOUPPER; RET(BLTIN); }
break;
case 65:

# line 146 "awk.lx.l"
{ yylval.i = FTOLOWER; RET(BLTIN); }
break;
case 66:

# line 147 "awk.lx.l"
	{ yylval.i = FLOG; RET(BLTIN); }
break;
case 67:

# line 148 "awk.lx.l"
	{ yylval.i = FINT; RET(BLTIN); }
break;
case 68:

# line 149 "awk.lx.l"
	{ yylval.i = FEXP; RET(BLTIN); }
break;
case 69:

# line 150 "awk.lx.l"
	{ yylval.i = FSQRT; RET(BLTIN); }
break;
case 70:

# line 151 "awk.lx.l"
	{ yylval.i = FSIN; RET(BLTIN); }
break;
case 71:

# line 152 "awk.lx.l"
	{ yylval.i = FCOS; RET(BLTIN); }
break;
case 72:

# line 153 "awk.lx.l"
{ yylval.i = FATAN; RET(BLTIN); }
break;
case 73:

# line 154 "awk.lx.l"
{ yylval.i = FSYSTEM; RET(BLTIN); }
break;
case 74:

# line 155 "awk.lx.l"
	{ yylval.i = FRAND; RET(BLTIN); }
break;
case 75:

# line 156 "awk.lx.l"
{ yylval.i = FSRAND; RET(BLTIN); }
break;
case 76:

# line 158 "awk.lx.l"
{ int n, c;
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
break;
case 77:

# line 173 "awk.lx.l"
	{ BEGIN str; clen = 0; }
break;
case 78:

# line 175 "awk.lx.l"
	{ if (--bracecnt < 0) yyerror("extra }"); BEGIN sc; RET(';'); }
break;
case 79:

# line 176 "awk.lx.l"
	{ if (--brackcnt < 0) yyerror("extra ]"); RET(']'); }
break;
case 80:

# line 177 "awk.lx.l"
	{ if (--parencnt < 0) yyerror("extra )"); RET(')'); }
break;
case 81:

# line 179 "awk.lx.l"
	{ if (yytext[0] == '{') bracecnt++;
		  else if (yytext[0] == '[') brackcnt++;
		  else if (yytext[0] == '(') parencnt++;
		  RET(yylval.i = yytext[0]); /* everything else */ }
break;
case 82:

# line 184 "awk.lx.l"
{ cbuf[clen++] = '\\'; cbuf[clen++] = yytext[1]; }
break;
case 83:

# line 185 "awk.lx.l"
	{ yyerror("newline in regular expression %.10s...", cbuf); lineno++; BEGIN A; }
break;
case 84:

# line 186 "awk.lx.l"
{ BEGIN A;
		  cbuf[clen] = 0;
		  yylval.s = (uchar *) tostring(cbuf);
		  unput('/');
		  RET(REGEXPR); }
break;
case 85:

# line 191 "awk.lx.l"
	{ CADD; }
break;
case 86:

# line 193 "awk.lx.l"
	{ BEGIN A;
		  cbuf[clen] = 0; s = (uchar *) tostring(cbuf);
		  /* cbuf[clen] = ' '*/ ; cbuf[++clen] = 0;
		  yylval.cp = xmakenode(STRING, cbuf, 0.0);
		  RET(STRING); }
break;
case 87:

# line 198 "awk.lx.l"
	{ yyerror("newline in string %.10s...", cbuf); lineno++; BEGIN A; }
break;
case 88:

# line 199 "awk.lx.l"
{ cbuf[clen++] = '"'; }
break;
case 89:

# line 200 "awk.lx.l"
{ cbuf[clen++] = '\n'; }
break;
case 90:

# line 201 "awk.lx.l"
{ cbuf[clen++] = '\t'; }
break;
case 91:

# line 202 "awk.lx.l"
{ cbuf[clen++] = '\f'; }
break;
case 92:

# line 203 "awk.lx.l"
{ cbuf[clen++] = '\r'; }
break;
case 93:

# line 204 "awk.lx.l"
{ cbuf[clen++] = '\b'; }
break;
case 94:

# line 205 "awk.lx.l"
{ cbuf[clen++] = '\\'; }
break;
case 95:

# line 206 "awk.lx.l"
{ int n;
		  sscanf(yytext+1, "%o", &n); cbuf[clen++] = n; }
break;
case 96:

# line 208 "awk.lx.l"
{ cbuf[clen++] = yytext[1]; }
break;
case 97:

# line 209 "awk.lx.l"
	{ CADD; }
break;
case -1:
break;
default:
(void)fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

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
int yyvstop[] = {
0,

81,
0, 

5,
81,
0, 

1,
0, 

14,
81,
0, 

77,
81,
0, 

4,
81,
0, 

38,
81,
0, 

81,
0, 

81,
0, 

80,
81,
0, 

81,
0, 

81,
0, 

81,
0, 

81,
0, 

81,
0, 

40,
81,
0, 

6,
81,
0, 

18,
81,
0, 

33,
81,
0, 

22,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

81,
0, 

79,
81,
0, 

35,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

76,
81,
0, 

81,
0, 

78,
81,
0, 

16,
81,
0, 

97,
0, 

87,
0, 

86,
97,
0, 

97,
0, 

85,
0, 

83,
0, 

84,
85,
0, 

85,
0, 

5,
0, 

15,
0, 

17,
0, 

4,
0, 

4,
0, 

4,
0, 

36,
0, 

37,
0, 

30,
0, 

12,
0, 

34,
0, 

28,
0, 

24,
0, 

26,
0, 

25,
0, 

27,
0, 

40,
0, 

29,
0, 

40,
0, 

40,
0, 

19,
0, 

20,
0, 

21,
0, 

23,
0, 

76,
0, 

76,
0, 

76,
0, 

39,
76,
0, 

7,
0, 

31,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

43,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

44,
76,
0, 

59,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

13,
0, 

96,
0, 

88,
96,
0, 

95,
96,
0, 

94,
96,
0, 

93,
96,
0, 

91,
96,
0, 

89,
96,
0, 

92,
96,
0, 

90,
96,
0, 

82,
0, 

2,
4,
0, 

4,
0, 

32,
0, 

40,
0, 

76,
0, 

9,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

71,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

68,
76,
0, 

42,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

67,
76,
0, 

76,
0, 

66,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

70,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

55,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

95,
0, 

4,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

45,
76,
0, 

47,
76,
0, 

10,
76,
0, 

76,
0, 

56,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

46,
76,
0, 

76,
0, 

74,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

69,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

95,
0, 

4,
0, 

8,
76,
0, 

72,
76,
0, 

48,
76,
0, 

61,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

57,
76,
0, 

76,
0, 

58,
76,
0, 

50,
76,
0, 

76,
0, 

53,
76,
0, 

76,
0, 

75,
76,
0, 

76,
0, 

76,
0, 

76,
0, 

76,
0, 

41,
76,
0, 

4,
0, 

76,
0, 

62,
76,
0, 

76,
0, 

76,
0, 

63,
76,
0, 

51,
76,
0, 

11,
76,
0, 

76,
0, 

54,
76,
0, 

73,
76,
0, 

76,
0, 

76,
0, 

4,
0, 

76,
0, 

76,
0, 

60,
76,
0, 

52,
76,
0, 

65,
76,
0, 

64,
76,
0, 

4,
0, 

49,
76,
0, 

10,
76,
0, 

4,
0, 

3,
4,
0, 
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	3,13,	0,0,	
0,0,	9,63,	0,0,	0,0,	
0,0,	0,0,	3,14,	3,15,	
0,0,	9,63,	9,64,	0,0,	
0,0,	5,59,	14,67,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	5,59,	5,60,	0,0,	
0,0,	0,0,	37,96,	0,0,	
0,0,	0,0,	3,16,	3,17,	
3,18,	3,19,	3,20,	3,21,	
0,0,	14,67,	3,22,	3,23,	
3,24,	0,0,	3,25,	3,26,	
3,27,	3,28,	5,61,	9,65,	
9,63,	0,0,	6,61,	0,0,	
0,0,	71,0,	21,76,	0,0,	
3,29,	3,30,	3,31,	3,32,	
5,59,	23,77,	3,33,	3,34,	
24,79,	9,63,	3,35,	25,81,	
72,0,	10,65,	16,68,	0,0,	
0,0,	0,0,	0,0,	3,36,	
0,0,	5,59,	20,75,	27,84,	
23,78,	30,88,	24,80,	25,82,	
31,89,	32,90,	32,91,	34,93,	
36,95,	3,37,	3,38,	3,39,	
9,66,	39,97,	3,40,	3,41,	
3,42,	3,43,	3,44,	3,45,	
3,46,	35,94,	3,47,	77,140,	
5,62,	3,48,	3,49,	3,50,	
6,62,	3,51,	71,138,	3,52,	
3,53,	3,54,	10,66,	49,114,	
3,55,	50,115,	93,143,	94,144,	
55,126,	3,56,	3,57,	3,58,	
4,16,	4,17,	4,18,	4,19,	
4,20,	4,21,	41,99,	18,70,	
4,22,	4,23,	4,24,	16,69,	
4,25,	4,26,	4,27,	18,70,	
18,0,	40,98,	42,100,	44,104,	
43,102,	42,101,	51,116,	46,108,	
54,125,	48,112,	4,29,	4,30,	
4,31,	4,32,	43,103,	44,105,	
45,106,	4,34,	47,110,	48,113,	
4,35,	46,109,	45,107,	52,117,	
53,119,	56,127,	47,111,	52,118,	
98,145,	4,36,	83,87,	53,120,	
53,121,	53,122,	99,146,	100,147,	
53,123,	72,139,	18,70,	102,150,	
53,124,	101,148,	104,151,	4,37,	
4,38,	4,39,	101,149,	106,154,	
4,40,	4,41,	4,42,	4,43,	
4,44,	4,45,	4,46,	18,70,	
4,47,	18,71,	107,155,	4,48,	
4,49,	4,50,	83,87,	4,51,	
105,152,	4,52,	4,53,	4,54,	
108,156,	109,157,	4,55,	105,153,	
112,160,	113,161,	114,162,	4,56,	
4,57,	4,58,	115,163,	116,164,	
117,165,	118,166,	111,158,	119,167,	
121,170,	122,171,	123,172,	18,72,	
19,73,	19,73,	19,73,	19,73,	
19,73,	19,73,	19,73,	19,73,	
19,73,	19,73,	111,159,	124,173,	
126,176,	139,0,	143,179,	145,180,	
146,181,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	147,182,	
148,183,	150,184,	151,185,	19,74,	
152,186,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	19,74,	
19,74,	19,74,	19,74,	26,83,	
26,83,	26,83,	26,83,	26,83,	
26,83,	26,83,	26,83,	26,83,	
26,83,	28,85,	155,187,	28,86,	
28,86,	28,86,	28,86,	28,86,	
28,86,	28,86,	28,86,	28,86,	
28,86,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	120,168,	
28,87,	156,188,	139,178,	157,189,	
158,190,	120,169,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
28,87,	160,191,	162,192,	163,193,	
33,92,	164,194,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
33,92,	33,92,	33,92,	33,92,	
62,128,	165,195,	66,137,	166,196,	
70,70,	168,197,	169,198,	170,199,	
62,128,	62,0,	66,137,	66,0,	
70,70,	70,0,	73,73,	73,73,	
73,73,	73,73,	73,73,	73,73,	
73,73,	73,73,	73,73,	73,73,	
171,200,	172,201,	173,202,	174,203,	
175,204,	176,205,	179,208,	180,209,	
181,210,	62,129,	74,74,	74,74,	
74,74,	74,74,	74,74,	74,74,	
74,74,	74,74,	74,74,	74,74,	
182,211,	183,212,	184,213,	62,130,	
187,214,	66,137,	188,215,	70,70,	
85,85,	85,85,	85,85,	85,85,	
85,85,	85,85,	85,85,	85,85,	
85,85,	85,85,	190,216,	125,174,	
62,128,	191,217,	66,137,	192,218,	
70,70,	87,141,	178,0,	87,141,	
125,175,	85,87,	87,142,	87,142,	
87,142,	87,142,	87,142,	87,142,	
87,142,	87,142,	87,142,	87,142,	
138,138,	194,219,	196,220,	197,221,	
198,222,	200,223,	201,224,	62,131,	
138,138,	138,0,	202,225,	203,226,	
204,227,	62,132,	205,228,	207,0,	
212,230,	62,133,	213,231,	214,232,	
215,233,	85,87,	217,234,	219,235,	
220,236,	62,134,	222,237,	224,238,	
225,239,	62,135,	226,240,	62,136,	
130,177,	130,177,	130,177,	130,177,	
130,177,	130,177,	130,177,	130,177,	
130,177,	130,177,	227,241,	229,0,	
230,243,	232,244,	233,245,	138,138,	
141,142,	141,142,	141,142,	141,142,	
141,142,	141,142,	141,142,	141,142,	
141,142,	141,142,	237,246,	240,247,	
241,248,	242,0,	243,250,	244,251,	
138,138,	177,206,	177,206,	177,206,	
177,206,	177,206,	177,206,	177,206,	
177,206,	177,206,	177,206,	178,207,	
249,0,	252,0,	253,253,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	253,253,	253,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
207,229,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	253,253,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	229,242,	0,0,	0,0,	
0,0,	0,0,	253,253,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
242,249,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
249,252,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	252,253,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+0,	0,		0,	
yycrank+0,	0,		0,	
yycrank+-1,	0,		0,	
yycrank+-95,	yysvec+3,	0,	
yycrank+-16,	0,		0,	
yycrank+-20,	yysvec+5,	0,	
yycrank+0,	0,		0,	
yycrank+0,	0,		0,	
yycrank+-4,	0,		0,	
yycrank+-26,	yysvec+9,	0,	
yycrank+0,	0,		0,	
yycrank+0,	0,		0,	
yycrank+0,	0,		yyvstop+1,
yycrank+9,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+13,	0,		yyvstop+8,
yycrank+0,	0,		yyvstop+11,
yycrank+-134,	0,		yyvstop+14,
yycrank+184,	0,		yyvstop+17,
yycrank+21,	0,		yyvstop+20,
yycrank+20,	0,		yyvstop+22,
yycrank+0,	0,		yyvstop+24,
yycrank+23,	0,		yyvstop+27,
yycrank+25,	0,		yyvstop+29,
yycrank+26,	0,		yyvstop+31,
yycrank+259,	0,		yyvstop+33,
yycrank+22,	0,		yyvstop+35,
yycrank+271,	0,		yyvstop+37,
yycrank+0,	0,		yyvstop+40,
yycrank+24,	0,		yyvstop+43,
yycrank+27,	0,		yyvstop+46,
yycrank+28,	0,		yyvstop+49,
yycrank+281,	0,		yyvstop+52,
yycrank+22,	yysvec+33,	yyvstop+55,
yycrank+27,	yysvec+33,	yyvstop+58,
yycrank+22,	yysvec+33,	yyvstop+61,
yycrank+20,	0,		yyvstop+64,
yycrank+0,	0,		yyvstop+66,
yycrank+36,	0,		yyvstop+69,
yycrank+29,	yysvec+33,	yyvstop+72,
yycrank+20,	yysvec+33,	yyvstop+75,
yycrank+38,	yysvec+33,	yyvstop+78,
yycrank+47,	yysvec+33,	yyvstop+81,
yycrank+39,	yysvec+33,	yyvstop+84,
yycrank+49,	yysvec+33,	yyvstop+87,
yycrank+50,	yysvec+33,	yyvstop+90,
yycrank+60,	yysvec+33,	yyvstop+93,
yycrank+52,	yysvec+33,	yyvstop+96,
yycrank+22,	yysvec+33,	yyvstop+99,
yycrank+20,	yysvec+33,	yyvstop+102,
yycrank+36,	yysvec+33,	yyvstop+105,
yycrank+70,	yysvec+33,	yyvstop+108,
yycrank+63,	yysvec+33,	yyvstop+111,
yycrank+41,	yysvec+33,	yyvstop+114,
yycrank+20,	yysvec+33,	yyvstop+117,
yycrank+45,	0,		yyvstop+120,
yycrank+0,	0,		yyvstop+122,
yycrank+0,	0,		yyvstop+125,
yycrank+0,	0,		yyvstop+128,
yycrank+0,	0,		yyvstop+130,
yycrank+0,	0,		yyvstop+132,
yycrank+-403,	0,		yyvstop+135,
yycrank+0,	0,		yyvstop+137,
yycrank+0,	0,		yyvstop+139,
yycrank+0,	0,		yyvstop+141,
yycrank+-405,	0,		yyvstop+144,
yycrank+0,	yysvec+14,	yyvstop+146,
yycrank+0,	0,		yyvstop+148,
yycrank+0,	0,		yyvstop+150,
yycrank+-407,	0,		yyvstop+152,
yycrank+-47,	yysvec+70,	yyvstop+154,
yycrank+-62,	yysvec+70,	yyvstop+156,
yycrank+370,	0,		yyvstop+158,
yycrank+390,	yysvec+19,	yyvstop+160,
yycrank+0,	0,		yyvstop+162,
yycrank+0,	0,		yyvstop+164,
yycrank+46,	0,		yyvstop+166,
yycrank+0,	0,		yyvstop+168,
yycrank+0,	0,		yyvstop+170,
yycrank+0,	0,		yyvstop+172,
yycrank+0,	0,		yyvstop+174,
yycrank+0,	0,		yyvstop+176,
yycrank+105,	yysvec+26,	yyvstop+178,
yycrank+0,	0,		yyvstop+180,
yycrank+408,	0,		yyvstop+182,
yycrank+0,	yysvec+28,	yyvstop+184,
yycrank+430,	0,		0,	
yycrank+0,	0,		yyvstop+186,
yycrank+0,	0,		yyvstop+188,
yycrank+0,	0,		yyvstop+190,
yycrank+0,	0,		yyvstop+192,
yycrank+0,	yysvec+33,	yyvstop+194,
yycrank+51,	yysvec+33,	yyvstop+196,
yycrank+55,	yysvec+33,	yyvstop+198,
yycrank+0,	yysvec+33,	yyvstop+200,
yycrank+0,	0,		yyvstop+203,
yycrank+0,	0,		yyvstop+205,
yycrank+75,	yysvec+33,	yyvstop+207,
yycrank+77,	yysvec+33,	yyvstop+209,
yycrank+68,	yysvec+33,	yyvstop+211,
yycrank+75,	yysvec+33,	yyvstop+213,
yycrank+75,	yysvec+33,	yyvstop+215,
yycrank+0,	yysvec+33,	yyvstop+217,
yycrank+71,	yysvec+33,	yyvstop+220,
yycrank+103,	yysvec+33,	yyvstop+222,
yycrank+77,	yysvec+33,	yyvstop+224,
yycrank+92,	yysvec+33,	yyvstop+226,
yycrank+96,	yysvec+33,	yyvstop+228,
yycrank+96,	yysvec+33,	yyvstop+230,
yycrank+0,	yysvec+33,	yyvstop+232,
yycrank+126,	yysvec+33,	yyvstop+235,
yycrank+106,	yysvec+33,	yyvstop+238,
yycrank+114,	yysvec+33,	yyvstop+240,
yycrank+102,	yysvec+33,	yyvstop+242,
yycrank+102,	yysvec+33,	yyvstop+244,
yycrank+118,	yysvec+33,	yyvstop+246,
yycrank+114,	yysvec+33,	yyvstop+248,
yycrank+109,	yysvec+33,	yyvstop+250,
yycrank+117,	yysvec+33,	yyvstop+252,
yycrank+231,	yysvec+33,	yyvstop+254,
yycrank+114,	yysvec+33,	yyvstop+256,
yycrank+132,	yysvec+33,	yyvstop+258,
yycrank+132,	yysvec+33,	yyvstop+260,
yycrank+128,	yysvec+33,	yyvstop+262,
yycrank+359,	yysvec+33,	yyvstop+264,
yycrank+139,	yysvec+33,	yyvstop+266,
yycrank+0,	0,		yyvstop+268,
yycrank+0,	0,		yyvstop+270,
yycrank+0,	0,		yyvstop+272,
yycrank+472,	0,		yyvstop+275,
yycrank+0,	0,		yyvstop+278,
yycrank+0,	0,		yyvstop+281,
yycrank+0,	0,		yyvstop+284,
yycrank+0,	0,		yyvstop+287,
yycrank+0,	0,		yyvstop+290,
yycrank+0,	0,		yyvstop+293,
yycrank+0,	0,		yyvstop+296,
yycrank+-487,	0,		yyvstop+298,
yycrank+-235,	yysvec+70,	yyvstop+301,
yycrank+0,	0,		yyvstop+303,
yycrank+488,	0,		0,	
yycrank+0,	yysvec+141,	yyvstop+305,
yycrank+173,	yysvec+33,	yyvstop+307,
yycrank+0,	yysvec+33,	yyvstop+309,
yycrank+137,	yysvec+33,	yyvstop+312,
yycrank+151,	yysvec+33,	yyvstop+314,
yycrank+160,	yysvec+33,	yyvstop+316,
yycrank+160,	yysvec+33,	yyvstop+318,
yycrank+0,	yysvec+33,	yyvstop+320,
yycrank+176,	yysvec+33,	yyvstop+323,
yycrank+177,	yysvec+33,	yyvstop+325,
yycrank+164,	yysvec+33,	yyvstop+327,
yycrank+0,	yysvec+33,	yyvstop+329,
yycrank+0,	yysvec+33,	yyvstop+332,
yycrank+219,	yysvec+33,	yyvstop+335,
yycrank+233,	yysvec+33,	yyvstop+337,
yycrank+245,	yysvec+33,	yyvstop+339,
yycrank+243,	yysvec+33,	yyvstop+341,
yycrank+0,	yysvec+33,	yyvstop+343,
yycrank+270,	yysvec+33,	yyvstop+346,
yycrank+0,	yysvec+33,	yyvstop+348,
yycrank+275,	yysvec+33,	yyvstop+351,
yycrank+259,	yysvec+33,	yyvstop+353,
yycrank+267,	yysvec+33,	yyvstop+355,
yycrank+305,	yysvec+33,	yyvstop+357,
yycrank+290,	yysvec+33,	yyvstop+359,
yycrank+0,	yysvec+33,	yyvstop+361,
yycrank+304,	yysvec+33,	yyvstop+364,
yycrank+305,	yysvec+33,	yyvstop+366,
yycrank+295,	yysvec+33,	yyvstop+368,
yycrank+318,	yysvec+33,	yyvstop+370,
yycrank+314,	yysvec+33,	yyvstop+372,
yycrank+314,	yysvec+33,	yyvstop+375,
yycrank+320,	yysvec+33,	yyvstop+377,
yycrank+320,	yysvec+33,	yyvstop+379,
yycrank+325,	yysvec+33,	yyvstop+381,
yycrank+505,	0,		yyvstop+383,
yycrank+-464,	yysvec+70,	yyvstop+385,
yycrank+356,	yysvec+33,	yyvstop+387,
yycrank+385,	yysvec+33,	yyvstop+389,
yycrank+329,	yysvec+33,	yyvstop+391,
yycrank+347,	yysvec+33,	yyvstop+393,
yycrank+344,	yysvec+33,	yyvstop+395,
yycrank+334,	yysvec+33,	yyvstop+397,
yycrank+0,	yysvec+33,	yyvstop+399,
yycrank+0,	yysvec+33,	yyvstop+402,
yycrank+336,	yysvec+33,	yyvstop+405,
yycrank+349,	yysvec+33,	yyvstop+408,
yycrank+0,	yysvec+33,	yyvstop+410,
yycrank+346,	yysvec+33,	yyvstop+413,
yycrank+353,	yysvec+33,	yyvstop+415,
yycrank+367,	yysvec+33,	yyvstop+417,
yycrank+0,	yysvec+33,	yyvstop+419,
yycrank+373,	yysvec+33,	yyvstop+422,
yycrank+0,	yysvec+33,	yyvstop+424,
yycrank+376,	yysvec+33,	yyvstop+427,
yycrank+375,	yysvec+33,	yyvstop+429,
yycrank+382,	yysvec+33,	yyvstop+431,
yycrank+0,	yysvec+33,	yyvstop+433,
yycrank+393,	yysvec+33,	yyvstop+436,
yycrank+378,	yysvec+33,	yyvstop+438,
yycrank+397,	yysvec+33,	yyvstop+440,
yycrank+380,	yysvec+33,	yyvstop+442,
yycrank+388,	yysvec+33,	yyvstop+444,
yycrank+401,	yysvec+33,	yyvstop+446,
yycrank+0,	0,		yyvstop+448,
yycrank+-493,	yysvec+70,	yyvstop+450,
yycrank+0,	yysvec+33,	yyvstop+452,
yycrank+0,	yysvec+33,	yyvstop+455,
yycrank+0,	yysvec+33,	yyvstop+458,
yycrank+0,	yysvec+33,	yyvstop+461,
yycrank+394,	yysvec+33,	yyvstop+464,
yycrank+405,	yysvec+33,	yyvstop+466,
yycrank+402,	yysvec+33,	yyvstop+468,
yycrank+398,	yysvec+33,	yyvstop+470,
yycrank+0,	yysvec+33,	yyvstop+472,
yycrank+406,	yysvec+33,	yyvstop+475,
yycrank+0,	yysvec+33,	yyvstop+477,
yycrank+409,	yysvec+33,	yyvstop+480,
yycrank+402,	yysvec+33,	yyvstop+483,
yycrank+0,	yysvec+33,	yyvstop+485,
yycrank+398,	yysvec+33,	yyvstop+488,
yycrank+0,	yysvec+33,	yyvstop+490,
yycrank+401,	yysvec+33,	yyvstop+493,
yycrank+407,	yysvec+33,	yyvstop+495,
yycrank+417,	yysvec+33,	yyvstop+497,
yycrank+429,	yysvec+33,	yyvstop+499,
yycrank+0,	yysvec+33,	yyvstop+501,
yycrank+-521,	yysvec+70,	yyvstop+504,
yycrank+415,	yysvec+33,	yyvstop+506,
yycrank+0,	yysvec+33,	yyvstop+508,
yycrank+422,	yysvec+33,	yyvstop+511,
yycrank+433,	yysvec+33,	yyvstop+513,
yycrank+0,	yysvec+33,	yyvstop+515,
yycrank+0,	yysvec+33,	yyvstop+518,
yycrank+0,	yysvec+33,	yyvstop+521,
yycrank+444,	yysvec+33,	yyvstop+524,
yycrank+0,	yysvec+33,	yyvstop+526,
yycrank+0,	yysvec+33,	yyvstop+529,
yycrank+433,	yysvec+33,	yyvstop+532,
yycrank+434,	yysvec+33,	yyvstop+534,
yycrank+-539,	yysvec+70,	yyvstop+536,
yycrank+449,	yysvec+33,	yyvstop+538,
yycrank+441,	yysvec+33,	yyvstop+540,
yycrank+0,	yysvec+33,	yyvstop+542,
yycrank+0,	yysvec+33,	yyvstop+545,
yycrank+0,	yysvec+33,	yyvstop+548,
yycrank+0,	yysvec+33,	yyvstop+551,
yycrank+-554,	yysvec+70,	yyvstop+554,
yycrank+0,	yysvec+33,	yyvstop+556,
yycrank+0,	yysvec+33,	yyvstop+559,
yycrank+-555,	yysvec+70,	yyvstop+562,
yycrank+-565,	0,		yyvstop+564,
0,	0,	0};
struct yywork *yytop = yycrank+671;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
  0,   1,   1,   1,   1,   1,   1,   1, 
  1,   9,  10,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  9,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,   1,   1,   1,   1,   1,   1, 
  1,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,   1,   1,   1,   1,  65, 
  1,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	Copyright (c) 1989 AT&T	*/
/*	  All Rights Reserved  	*/

// Note: This software was created by the Bell Laboratories unit of AT&T.
// Bell Laboratories was subsequently part of Lucent Technologies, later part of
// Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
// by AT&T to its successors. This license is granted by Nokia solely to the
// extent of its right to do so.

#pragma ident	"@(#)ncform	6.8	95/02/11 SMI"

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int yylook(void)
#else
yylook()
#endif
{
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
#ifndef __cplusplus
			*yylastch++ = yych = input();
#else
			*yylastch++ = yych = lex_input();
#endif
			if(yylastch > &yytext[YYLMAX]) {
				fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
#ifndef __cplusplus
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
#else
		yyprevious = yytext[0] = lex_input();
		if (yyprevious>0)
			lex_output(yyprevious);
#endif
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
#if defined(__cplusplus) || defined(__STDC__)
int yyback(int *p, int m)
#else
yyback(p, m)
	int *p;
#endif
{
	if (p==0) return(0);
	while (*p) {
		if (*p++ == m)
			return(1);
	}
	return(0);
}
	/* the following are only used in the lex library */
#if defined(__cplusplus) || defined(__STDC__)
int yyinput(void)
#else
yyinput()
#endif
{
#ifndef __cplusplus
	return(input());
#else
	return(lex_input());
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyoutput(int c)
#else
yyoutput(c)
  int c; 
#endif
{
#ifndef __cplusplus
	output(c);
#else
	lex_output(c);
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyunput(int c)
#else
yyunput(c)
   int c; 
#endif
{
	unput(c);
	}
