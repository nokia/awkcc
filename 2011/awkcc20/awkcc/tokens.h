/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

/* Regular expression possibilities */

#define RCARAT	1
#define RREGEXP	2
#define RREGEX	4
#define RNORMAL	8

/* Functions */

#define FATAN	1001
#define FEXP	1002
#define FLENGTH	1003
#define FSYSTEM	1004
#define FCOS	1005
#define FINT	1006
#define FLOG	1007
#define FRAND	1008
#define FSIN	1009
#define FSQRT	1010
#define FSRAND	1011
#define FTOUPPER	1012
#define FTOLOWER	1013

/* Superstatements */

#define COMPSTMT	1500
#define	INWHILE	1501

/* constant types */

#define REGEXP	1503
#define FLOAT	1504
#define INT	1505
#define EXTERN	1506
#define UVAR	1507

/* Variable types */

#define SYSVAR	1601
#define SYSARRAY	1602
#define FLOATVAR	1603
#define INTVAR	1604
#define PARAMETER	1605
#define VARPARAM	1606
#define ARRPARAM	1607

/* Functions */

#define USEDCALL	1608

/* Redirection */

#define PIPE	1611

/* Type information */

#define YNUM	0
#define YINT	1
#define YSTR	2
#define YBTH	3
#define YSTRUNK	4
#define YNUMUNK	5
#define YUNK	6
#define YDEP	7
#define YBOOL	8

#define ENDIT	9

/* BOOLEAN */

#define TRUE	1
#define FALSE	0

/* FLAGS */
#define FIXORD	1	/* order of evaluation fix */
#define HASRET	2	/* function has a return? 		*/
#define PRECONV	4	/* +=, -=, etc:  pre-conv lval to num?	*/
#define FELEM	8	/* flag for within IN loops		*/

#define FREGEXP	1	/* regexp type, flag in varinfo		*/
#define FGLOBL	2	/* used globally in function, varinfo	*/

/* For symbols which have not been set as functions, arrays, etc. */

#define SYM	2048

/* STACK OPERATIONS */

#define INVERT	1
#define POP	2
#define PUSH	3
#define POPR	4
#define NULLIFY	5
#define FIND 	6
#define CHECKOK	7
#define AFFECT	8
#define AFFECTF	9

#define CSTACK	0
#define CBSTACK	1
#define CBFSTACK	2


/* Garbage that needs to get into the yacc */
#define NIL	((Node *) NULL)
#define makearr(a)	a
#define ER		sprintf(errbuf,
#define SX		), yyerror(errbuf)
