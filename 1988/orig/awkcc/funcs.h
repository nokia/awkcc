
/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/
extern Nodep	compstmt();
extern Nodep	checkfunc();
extern uchar	*malloc();
extern uchar	*calloc();
extern uchar	*realloc();
extern VARINFOP	memq();
extern VARINFOP	fnmemq();
extern double	atof();
extern Nodep	makenode();
extern Nodep	pparams();
extern Nodep	stmt1();
extern Nodep	prog();
extern FILE	*fopen();
extern VARINFOP	make_varinfo();
extern Nodep	thearg();
extern Nodep	rarglist();
extern Nodep	genstat();
extern Nodep	revlinks();
extern Nodep	linkum();
extern Nodep	exptostat();
extern Nodep	rectonode();
extern uchar	*tostring();
extern Nodep	xmakenode();
extern Nodep	pa2stat();

#define stat4(t,t1,t2,t3,t4)	genstat(1,4,t,t1,t2,t3,t4)
#define stat3(t,t1,t2,t3)	genstat(1,3,t,t1,t2,t3)
#define stat2(t,t1,t2)		genstat(1,2,t,t1,t2)
#define stat1(t,t1)		genstat(1,1,t,t1)
#define stat0(t)		genstat(1,0,t)

#define op3(t,t1,t2,t3)		genstat(0,3,t,t1,t2,t3)
#define op2(t,t1,t2)		genstat(0,2,t,t1,t2)
#define op1(t,t1)		genstat(0,1,t,t1)
#define op0(t)			genstat(0,0,t)

extern char	errbuf[];
