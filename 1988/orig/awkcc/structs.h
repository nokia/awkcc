/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/

typedef unsigned char	uchar;

typedef struct node {

	/* TYPE INFORMATION */
	short	y;		/* the current type 			*/
	short	w;		/* what we want it to yield 		*/
	short 	t;		/* availabiliby indicator 		*/
	short	rt;		/* real type 				*/

	/* COUNTERS */
	short	sympos;		/* position in symbol table 		*/
	short	called;		/* number of times function called	*/
	short	tempvar;	/* number of the temporary variable	*/
	short	tempstr;	/* number of the temporary string	*/
	short	tempfun;	/* number of funtemp 			*/
	short	tempif;		/* for ifelse's 			*/
	short	argnum;		/* for arguments			*/

	/* FLAGS */
	short	flags;

	/* USEFUL */
	double	n;		/* The numeric value (leaves only?)	*/
	uchar	*s;		/* The string value (leaves only)	*/

	/* MISCELLANEOUS */
	uchar	subs;		/* to keep track of narg's size 	*/
	short	ntype;		/* to make the yacc version happy 	*/
	short	lineno;		/* for error messages			*/

	/* OTHER NodeS */
	struct node	*nnext;
	struct node	**narg;
} Node, *Nodep;

#define TOKLEN	400

typedef struct {
	int 	type;
	double 	num;
	uchar 	string[TOKLEN];
	int	lineno;
} TOKEN, *TOKENP;
