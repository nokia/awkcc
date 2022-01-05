/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

/*  NUMERIC AMENTS TO SPECIAL VARIABLES
 */

#define ASUBSEPNUM(a,b)(Au_SUBSEP->cur=INITNUM,		\
			 Au_SUBSEP->num=b),			\
			 TS(Au_SUBSEP),			\
			 SS=S(Au_SUBSEP),			\
			 Au_SUBSEP->num)

#define AOFMTNUM(a,b)(Au_OFMT->cur=INITNUM,		\
			 Au_OFMT->num=b),			\
			 TS(Au_OFMT),			\
			 OFMT=S(Au_OFMT),			\
			 Au_OFMT->num)

#define AARGCNUM(a,b)((Au_ARGC->cur=INITNUM),		\
			   (Au_ARGC->num=b),			\
			   ((Au_ARGC->num>=0)?			\
			 	(ARGC=(int)(Au_ARGC->num)):	\
			 	(ARGC=ARGC)),			\
			   Au_ARGC->num)

#define ARSNUM(a,b)(Au_RS->cur=INITNUM,			\
			 (Au_RS->num=b),			\
			 TS(Au_RS),				\
			 (RS=S(Au_RS)),			\
			 ((*RS=='\n')?				\
				(rsdflt=0):			\
				((*RS)?				\
					(rsdflt=1):		\
					(rsdflt=2))),		\
			 Au_RS->num)

#define AFSNUM(a,b)(Au_FS->cur=INITNUM,			\
			 (Au_FS->num=b),			\
			 TS(Au_FS),				\
			 clearctest(*(S(Au_FS))),		\
			 ASTR(REALFS, S(Au_FS)),		\
			 (STRFS=S(REALFS)),			\
			 ((strlen(STRFS)>1)?			\
			 	(isreg(STRFS)?			\
					((REFS?freefa(REFS):0), \
					 (REFS=mdfa(reparse(STRFS), 1)),\
					 dflt=3):		\
					(fixreg(STRFS),dflt=2)):\
				((*STRFS==' ')?			\
					(setctest('\t', ctest),	\
					 setctest('\n', ctest),	\
					 setctest(' ', ctest),	\
					 (dflt=1)):		\
					(dflt=0))),		\
			 osplitrec(D[0], upf),			\
			 pristine=1,				\
			 assign_NF(),				\
			 Au_FS->num)

#define AORSNUM(a,b)(Au_ORS->cur=INITNUM,			\
			 Au_ORS->num=b),			\
			 TS(Au_ORS),			\
			 ORS=S(Au_ORS),			\
			 Au_ORS->num)

#define AOFSNUM(a,b)(Au_OFS->cur=INITNUM,			\
			 Au_OFS->num=b),			\
			 TS(Au_OFS),			\
			 OFS=S(Au_OFS),			\
			 Au_OFS->num)

/*  SPECIAL STRING AMENT
 */

#define ASUBSEPSTR(a,b)	(vassign_str(a,b),		\
				 SS=S(a))

#define AOFMTSTR(a,b)	(vassign_str(a,b),		\
				 OFMT=S(a))

#define AARGCSTR(a,b) (ANAS(Au_ARGC, b),	\
			    ((ISNUM(Au_ARGC->cur) && Au_ARGC->num>=0)?\
			 	 (ARGC=(int)Au_ARGC->num):	\
			 	 (ARGC=ARGC)),			\
			    S(Au_ARGC))

#define ARSSTR(a,b)(vassign_str(Au_RS, b),		\
			 (RS=S(Au_RS)),			\
			 ((*RS=='\n')?				\
				(rsdflt=0):			\
				((*RS)?				\
					(rsdflt=1):		\
					(rsdflt=2))),		\
			 S(Au_RS))

#define AFSSTR(a,b)(vassign_str(Au_FS, b),		\
			 clearctest(*(S(Au_FS))),		\
			 ASTR(REALFS, S(Au_FS)),		\
			 (STRFS=S(REALFS)),			\
			 ((strlen(STRFS)>1)?			\
			 	(isreg(STRFS)?			\
					((REFS?freefa(REFS):0), \
					 (REFS=mdfa(reparse(STRFS), 1)),\
					 dflt=3):		\
					(fixreg(STRFS),dflt=2)):\
				 ((*STRFS==' ')?		\
					(setctest('\t', ctest),	\
					 setctest('\n', ctest),	\
					 setctest(' ', ctest),		\
					 (dflt=1)):		\
					(dflt=0))),		\
			 osplitrec(D[0], upf),			\
			 pristine=1,				\
			 assign_NF(),				\
			 S(Au_FS))

#define AOFSSTR(a,b)(vassign_str(Au_OFS, b),		\
			  OFS=S(Au_OFS))

#define AORSSTR(a,b)(vassign_str(Au_ORS, b),		\
			  ORS=S(Au_ORS))

/* SPECIAL VARIABLE AMENT
 */

#define AOFSVARS(a,b) (assignvars(a,b), TS(a), OFS=(S(a)), a)

#define AORSVARS(a,b) (assignvars(a,b), TS(a), ORS=(S(a)), a)

#define AARGCVARS(a,b) (assignvars(a,b), 			\
			     TONUM(a),				\
			     ((a->num>=0)&&isitanum(Au_ARGC)|ISDFLD?\
			     	ARGC=(int)(a->num): 		\
			     	ARGC=ARGC),			\
			     a)

#define ARSVARS(a,b)(assignvars(a,b),			\
			 TS(a),					\
			 (RS=S(Au_RS)),			\
			 ((*RS=='\n')?				\
				(rsdflt=0):			\
				((*RS)?				\
					(rsdflt=1):		\
					(rsdflt=2))),		\
			 Au_RS->num)

#define AFSVARS(a,b) (assignvars(a,b), 			\
			   TS(a), 				\
			   clearctest(*(S(Au_FS))),		\
			   ASTR(REALFS, S(Au_FS)),		\
			   (STRFS=S(REALFS)),			\
			   ((strlen(STRFS)>1)?			\
			 	(isreg(STRFS)?			\
					((REFS?freefa(REFS):0), \
					 (REFS=mdfa(reparse(STRFS), 1)),\
					 dflt=3):		\
					(fixreg(STRFS),dflt=2)):\
				((*STRFS==' ')?			\
					(setctest('\t', ctest),	\
					 setctest('\n', ctest),	\
					 setctest(' ', ctest),		\
					 (dflt=1)):		\
					(dflt=0))),		\
			   osplitrec(D[0], upf),			\
			   pristine=1,				\
			   assign_NF(),				\
			   a)			\

#define AOFMTVARS(a,b) (assignvars(a,b), TS(a), OFMT=S(a), a)

#define ASUBSEPVARS(a,b) (assignvars(a,b), TS(a), SS=S(a), a)
