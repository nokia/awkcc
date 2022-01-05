
/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/
#define MAX_LEVELS	128
#define MAX_FORMAT	124

typedef struct varinfo {
	short	cur[MAX_LEVELS];	/* currently avail 		*/
	short	rt[MAX_LEVELS];		/* "real" type 			*/
	short	type;			/* what kind of symbol? 	*/
	uchar	*string;		/* symbol's name? 		*/
	uchar	*func;			/* if param, which func? 	*/
	short	flags;			/* REGEXP type in bit one	*/
	Nodep	*exprs;			/* $(x+1): expr<-x+1		*/
	short	ne;			/* # of exprs			*/
	short	sympos;			/* position in symbol table	*/
	short	argnum;			/* argument number, if arg	*/
	short	called;			/* times called, if param 	*/
	short	atyped;			/* save work in array typing	*/
} VARINFO, *VARINFOP;
	
#define MAX_ARRAYS	16
#define MAX_VARIABLES	124

#define LEFT	narg[0]
#define RIGHT	narg[1]
#define THREE	narg[2]
#define FOUR	narg[3]
