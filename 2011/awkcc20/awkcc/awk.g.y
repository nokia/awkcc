%{
#include <stdio.h>
#include "structs.h"
#include "parse.h"
#include "tokens.h"
#include "funcs.h"

#define ERROR	ER
#define SYNTAX	SX
extern int	errorflag;

yywrap() { return(1); }
#ifndef	DEBUG
#	define	PUTS(x)
#endif
Node	*beginloc = 0, *endloc = 0;
Node	*funcloc = 0;
int	infunc	= 0;	/* = 1 if in arglist or body of func */
uchar	*curfname = 0;
Node	*arglist = 0;	/* list of args for current function */
uchar	*strnode();

extern Nodep	winner;
%}

%union {
	Node	*p;
	Node	*cp;
	int	i;
	uchar	*s;
}

%token	<i>	FIRSTTOKEN	/* must be first */
%token	<p>	PROGRAM PASTAT PASTAT2 XBEGIN XEND
%token	<i>	NL ',' '{' '(' '|' ';' '/' ')' '}' '[' ']'
%token	<i>	ARRAY
%token	<i>	MATCH NOTMATCH MATCHOP
%token	<i>	FINAL DOT ALL CCL NCCL CHAR OR STAR QUEST PLUS
%token	<i>	AND BOR APPEND EQ GE GT LE LT NE IN
%token	<i>	ARG BLTIN BREAK CLOSE CONTINUE DELETE DO EXIT FOR FUNC 
%token	<i>	SUB GSUB IF INDEX LSUBSTR MATCHFCN NEXT
%token	<i>	ADD MINUS MULT DIVIDE MOD
%token	<i>	ASSIGN ASGNOP ADDEQ SUBEQ MULTEQ DIVEQ MODEQ POWEQ
%token	<i>	PRINT PRINTF SPRINTF
%token	<p>	ELSE INTEST CONDEXPR
%token	<i>	POSTINCR PREINCR POSTDECR PREDECR
%token	<p>	VAR IVAR VARNF CALL NUMBER STRING FIELD
%token	<s>	REGEXPR
%token	<p>	CLINE

%type	<p>	pas pattern ppattern plist pplist patlist prarg term
%type	<p>	pa_pat pa_stat pa_stats reg_expr
%type	<p>	simple_stmt opt_simple_stmt stmt stmtlist
%type	<p>	var varname funcname varlist
%type	<p>	for if while
%type	<i>	pst opt_pst lbrace rparen comma nl opt_nl and bor
%type	<i>	subop print

%right	ASGNOP
%right	'?'
%right	':'
%left	BOR
%left	AND
%left	GETLINE
%nonassoc APPEND EQ GE GT LE LT NE MATCHOP IN '|'
%left	ARG BLTIN BREAK CALL CLOSE CONTINUE DELETE DO EXIT FOR FIELD FUNC 
%left	GSUB IF INDEX LSUBSTR MATCHFCN NEXT NUMBER
%left	PRINT PRINTF RETURN SPLIT SPRINTF STRING SUB SUBSTR
%left	REGEXPR VAR VARNF IVAR WHILE '('
%left	CAT
%left	'+' '-'
%left	'*' '/' '%'
%left	NOT UMINUS
%right	POWER
%right	DECR INCR
%left	INDIRECT
%token	LASTTOKEN	/* must be last */

%%

program:
	  pas	{ if (errorflag==0)
			winner = (Node *)stat4(PROGRAM, beginloc, $1, endloc, funcloc);
		}
	| error	{ yyclearin; bracecheck(); ERROR "bailing out" SYNTAX; }
	;

and:
	  AND | and NL
	;

bor:
	  BOR | bor NL
	;

comma:
	  ',' | comma NL
	;

do:
	  DO | do NL
	;

else:
	  ELSE | else NL
	;

for:
	  FOR '(' opt_simple_stmt ';' pattern ';' opt_simple_stmt rparen stmt
		{ $$ = stat4(FOR, $3, $5, $7, $9); }
	| FOR '(' opt_simple_stmt ';'  ';' opt_simple_stmt rparen stmt
		{ $$ = stat4(FOR, $3, NIL, $6, $8); }
	| FOR '(' varname IN varname rparen stmt
		{ $$ = stat3(IN, $3, makearr($5), $7); }
	;

funcname:
	  VAR	{ setfname($1); }
	| CALL	{ setfname($1); }
	;

if:
	  IF '(' pattern rparen		{ $$ = $3; }
	;

lbrace:
	  '{' | lbrace NL
	;

nl:
	  NL | nl NL
	;

opt_nl:
	  /* empty */	{ $$ = 0; }
	| nl
	;

opt_pst:
	  /* empty */	{ $$ = 0; }
	| pst
	;


opt_simple_stmt:
	  /* empty */			{ $$ = 0; }
	| simple_stmt
	;

pas:
	  opt_pst			{ $$ = 0; }
	| opt_pst pa_stats opt_pst	{ $$ = $2; }
	;

pa_pat:
	  pattern	{ $$ = $1; }
	;

pa_stat:
	  pa_pat			
		{ $$ = stat2(PASTAT, $1, 
		  stat3(PRINT, rectonode(), NIL, NIL)); 
		}
	| pa_pat lbrace stmtlist '}'	{ $$ = stat2(PASTAT, $1, $3); }
	| pa_pat ',' pa_pat		
		{ 
		  $$ = stat3(PASTAT2, $1, $3, 
		  stat3(PRINT, rectonode(), NIL, NIL)); 
		}
	| pa_pat ',' pa_pat lbrace stmtlist '}'	{ $$ = stat3(PASTAT2, $1, $3, $5); }
	| lbrace stmtlist '}'		{ $$ = stat2(PASTAT, NIL, $2); }
	| XBEGIN lbrace stmtlist '}'
		{ beginloc = linkum(beginloc, stat2(PASTAT, NIL, $3)); $$ = 0; }
	| XEND lbrace stmtlist '}'
		{ endloc = linkum(endloc, stat2(PASTAT, NIL, $3)); $$ = 0; }
	| FUNC funcname '(' varlist rparen {infunc++;setargnames($4);} lbrace stmtlist '}'
		{ 
			infunc--; curfname=0;
/* JCR */
			funcloc = linkum(funcloc, defn($2, $4, $8));
			$$=0;
		}
	;

pa_stats:
	  pa_stat
	| pa_stats pst pa_stat	{ $$ = linkum($1, $3); }
	;

patlist:
	  pattern
	| patlist comma pattern	{ $$ = linkum($1, $3); }
	;

ppattern:
	  var ASGNOP ppattern		{ $$ = op2($2, $1, $3); }
	| ppattern '?' ppattern ':' ppattern %prec '?'
	 	{ $$ = op3(CONDEXPR, $1, $3, $5); }
	| ppattern bor ppattern %prec BOR
		{ $$ = op2(BOR, $1, $3); }
	| ppattern and ppattern %prec AND
		{ $$ = op2(AND, $1, $3); }
	| NOT ppattern
		{ $$ = op1(NOT, $2); }
	| ppattern MATCHOP reg_expr	{ $$ = op2($2, $1, $3); }
	| ppattern MATCHOP ppattern
		{
			$$ = op2($2, $1, $3);
		}
	| ppattern IN varname		{ $$ = op2(INTEST, $1, makearr($3)); }
	| '(' plist ')' IN varname	{ $$ = op2(INTEST, $2, makearr($5)); }
	| ppattern term %prec CAT	{ $$ = op2(CAT, $1, $2); }
	| reg_expr
		{ $$ = op2(MATCH, rectonode(), $1); }
	| term
	;

pattern:
	  var ASGNOP pattern		{ $$ = op2($2, $1, $3); }
	| pattern '?' pattern ':' pattern %prec '?'
	 	{ $$ = op3(CONDEXPR, $1, $3, $5); }
	| pattern bor pattern %prec BOR
		{ $$ = op2(BOR, $1, $3); }
	| pattern and pattern %prec AND
		{ $$ = op2(AND, $1, $3); }
	| NOT pattern 			{ $$ = op1(NOT, $2); }
	| pattern EQ pattern		{ $$ = op2($2, $1, $3); }
	| pattern GE pattern		{ $$ = op2($2, $1, $3); }
	| pattern GT pattern		{ $$ = op2($2, $1, $3); }
	| pattern LE pattern		{ $$ = op2($2, $1, $3); }
	| pattern LT pattern		{ $$ = op2($2, $1, $3); }
	| pattern NE pattern		{ $$ = op2($2, $1, $3); }
	| pattern MATCHOP reg_expr	{ $$ = op2($2, $1, $3); }
	| pattern MATCHOP pattern
		{ 
			$$ = op2($2, $1,$3);
		}
	| pattern IN varname		{ $$ = op2(INTEST, $1, makearr($3)); }
	| '(' plist ')' IN varname	{ $$ = op2(INTEST, $2, makearr($5)); }
	| pattern '|' GETLINE var	{ $$ = op3(GETLINE, $4, (Node*)$2, $1); }
	| pattern '|' GETLINE		{ $$ = op3(GETLINE, (Node*)0, (Node*)$2, $1); }
	| pattern term %prec CAT	{ $$ = op2(CAT, $1, $2); }
	| reg_expr
		{ $$ = op2(MATCH, rectonode(), $1); }
	| term 
	;

plist:
	  pattern comma pattern		{ $$ = linkum($1, $3); }
	| plist comma pattern		{ $$ = linkum($1, $3); }
	;

pplist:
	  ppattern
	| pplist comma ppattern		{ $$ = linkum($1, $3); }

prarg:
	  /* empty */			{ $$ = rectonode(); }
	| pplist
	| '(' plist ')'			{ $$ = $2; }
	;

print:
	  PRINT | PRINTF
	;

pst:
	  NL | ';' | pst NL | pst ';'
	;

rbrace:
	  '}' | rbrace NL
	;

reg_expr:
	  '/' {startreg();} REGEXPR '/'		{ $$ = xmakenode(REGEXPR, $3, 0.0); }
	;

rparen:
	  ')' | rparen NL
	;

simple_stmt:
	  print prarg '|' term		{ $$ = stat3($1, $2, (Node *) $3, $4); }
	| print prarg APPEND term	{ $$ = stat3($1, $2, (Node *) $3, $4); }
	| print prarg GT term		{ $$ = stat3($1, $2, (Node *) $3, $4); }
	| print prarg			{ $$ = stat3($1, $2, NIL, NIL); }
	| DELETE varname '[' patlist ']' { $$ = stat2(DELETE, makearr($2), $4); }
	| pattern			{ $$ = exptostat($1); }
	| error				{ yyclearin; ERROR "illegal statement" SYNTAX; }
	;

st:
	  nl | ';' opt_nl
	;

stmt:
	  BREAK st		{ $$ = stat1(BREAK, NIL); }
	| CLOSE pattern st	{ $$ = stat1(CLOSE, $2); }
	| CONTINUE st		{ $$ = stat1(CONTINUE, NIL); }
	| do stmt WHILE '(' pattern ')' st
		{ $$ = stat2(DO, $2, $5); }
	| EXIT pattern st	{ $$ = stat1(EXIT, $2); }
	| EXIT st		{ $$ = stat1(EXIT, NIL); }
	| for
	| if stmt else stmt	{ $$ = stat3(IF, $1, $2, $4); }
	| if stmt		{ $$ = stat3(IF, $1, $2, NIL); }
	| lbrace stmtlist rbrace { $$ = $2; }
	| NEXT st	{ if (infunc)
				ERROR "next is illegal inside a function" SYNTAX;
			$$ = stat1(NEXT, NIL); }
	| RETURN pattern st	{ $$ = stat1(RETURN, $2); }
	| RETURN st		{ $$ = stat1(RETURN, NIL); }
	| simple_stmt st
	| while stmt		{ $$ = stat2(WHILE, $1, $2); }
	| ';' opt_nl		{ $$ = 0; }
	| CLINE nl		{ $$ = $1; }
	;

stmtlist:
	  stmt
	| stmtlist stmt		{ $$ = linkum($1, $2); }
	;

subop:
	  SUB | GSUB
	;

term:
	  term '+' term			{ $$ = op2(ADD, $1, $3); }
	| term '-' term			{ $$ = op2(MINUS, $1, $3); }
	| term '*' term			{ $$ = op2(MULT, $1, $3); }
	| term '/' term			{ $$ = op2(DIVIDE, $1, $3); }
	| term '%' term			{ $$ = op2(MOD, $1, $3); }
	| term POWER term		{ $$ = op2(POWER, $1, $3); }
	| '-' term %prec UMINUS		{ $$ = op1(UMINUS, $2); }
	| '+' term %prec UMINUS		{ $$ = $2; }
	| BLTIN '(' ')'			{ $$ = op1($1, rectonode()); }
	| BLTIN '(' patlist ')'		{ $$ = op1($1, $3); }
	| BLTIN				{ $$ = op1($1, rectonode()); }
	| CALL '(' ')'			{ 
					  subsize($1, 1);
					  $1->LEFT=(Nodep) NULL;
					  add_func($1->s);
					}
	| CALL '(' patlist ')'		{
					  subsize($1, 1);
					  $1->LEFT=$3;
					  add_func($1->s);
					  setfargnames($1);
					}
	| DECR var			{ $$ = op1(PREDECR, $2); }
	| INCR var			{ $$ = op1(PREINCR, $2); }
	| var DECR			{ $$ = op1(POSTDECR, $1); }
	| var INCR			{ $$ = op1(POSTINCR, $1); }
	| GETLINE var LT term		{ $$ = op3(GETLINE, $2, (Node *)$3, $4); }
	| GETLINE LT term		{ $$ = op3(GETLINE, NIL, (Node *)$2, $3); }
	| GETLINE var			{ $$ = op3(GETLINE, $2, NIL, NIL); }
	| GETLINE			{ $$ = op3(GETLINE, NIL, NIL, NIL); }
	| INDEX '(' pattern comma pattern ')'
		{ $$ = op2(INDEX, $3, $5); }
	| INDEX '(' pattern comma reg_expr ')'
		{ ERROR "index() doesn't permit regular expressions" SYNTAX;
		  $$ = op2(INDEX, $3, $5); }
	| '(' pattern ')'		{ $$ = $2; }
	| MATCHFCN '(' pattern comma reg_expr ')'
		{ $$ = op2(MATCHFCN, $3, $5); }
	| MATCHFCN '(' pattern comma pattern ')'
		{ 
			$$ = op2(MATCHFCN, $3, $5);
		}
	| NUMBER			{ $$ = $1; }
	| SPLIT '(' pattern comma varname comma pattern ')'
		{ $$ = op3(SPLIT, $3, makearr($5), $7); }
	| SPLIT '(' pattern comma varname comma reg_expr ')'
		{ $$ = op3(SPLIT, $3, makearr($5), $7); }
	| SPLIT '(' pattern comma varname ')'
		{ $$ = op3(SPLIT, $3, makearr($5), (Node*) 0); }
	| SPRINTF '(' patlist ')'	{ $$ = op1($1, $3); }
	| STRING	 		{ $$ = $1; }
	| subop '(' reg_expr comma pattern ')'
		{ $$ = op3($1, $3, $5, rectonode()); }
	| subop '(' pattern comma pattern ')'
		{
			$$ = op3($1, $3, $5, rectonode());
		}
	| subop '(' reg_expr comma pattern comma pattern ')'
		{ $$ = op3($1, $3, $5, $7); } 
	| subop '(' pattern comma pattern comma pattern ')'
		{
			$$ = op3($1, $3, $5, $7); 
		}
	| SUBSTR '(' pattern comma pattern comma pattern ')'
		{ $$ = op3(SUBSTR, $3, $5, $7); }
	| SUBSTR '(' pattern comma pattern ')'
		{ $$ = op3(SUBSTR, $3, $5, NIL); }
	| var 
	;
var:
	  varname			{ /* setvarname($1);*/ }
	| varname '[' patlist ']'	{ subsize($1, 1);
					  $1->LEFT=$3;
				 	  setarrname($1);
					  makearr($1);
				   	  $$=$1;
					}
	| FIELD				{ $$ = $1; }
	| IVAR				{ $$ = $1; ERROR "Not used in awkcc" SYNTAX; }
	| INDIRECT term	 		{ $$ = op1(INDIRECT, $2); }
	;	

varlist:
	  /* nothing */		{ arglist = $$ = 0; }
	| VAR			{ arglist = $$ = $1; }
	| varlist comma VAR	{ arglist = $$ = linkum($1, $3); }
	;

varname:
	  VAR			{ $$ = $1; }
	| ARG 			{ 
				  Nodep	tmp;
				  tmp=makenode(ARG,-107,(TOKENP) NULL);
				  tmp->ntype=0;
				  subsize(tmp, 1);
				  tmp->argnum=$1;
				  fix(tmp);
				  tmp->s=thearg(arglist, $1)->s;
				  $$=tmp;
				}
	| VARNF			{ $$ = $1; }
	;


while:
	  WHILE '(' pattern rparen	{ $$ = $3; }
	;

%%
