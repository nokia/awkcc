/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/

/* RETRIEVAL
 */

#define GETDVARS(change, a)	(getd(a, change))

#define GETDSTR(change, a)	(TS(getd(a, change)))

#define GETDNUM(change, a)	(TONUM(getd(a, change)))

#define VGETDVARS(change, a)	(riggetd(a, change, &i))

#define VGETDSTR(change, a)	(TS(riggetd(a, change, &i)))

#define VGETDNUM(change, a)	(TONUM(riggetd(a, change, &i)))


/* $0 STUFF 
 */

#define GETD0STR	(pristine?S(D[0]):S(mD0()))

#define GETD0NUM	(pristine?TONUM(D[0]):(mD0(),TONUM(D[0])))

#define GETD0VARS	(pristine?D[0]:(mD0(), D[0])	

/* AMENT */

#define VADNUM(a,b) ((tmpnum=b),				\
			 ((riggetd(a, 1, &i), i)?		\
				((D[i]->cur=INITNUM),		\
			 	 (pristine=0),			\
				 (D[i]->num=tmpnum),		\
				 tmpnum):			\
				((D[0]->num=tmpnum),		\
				 (D[0]->cur=INITNUM),		\
				 ToStr(D[0]),			\
				 AD0, 			\
				 tmpnum)))

#define VADSTR(a,b) ((tmpchar=b),				\
			 ((riggetd(a, 1, &i), i)?		\
			 	(vassign_str(D[i], tmpchar),\
			 	 (pristine=0),			\
				 S(D[i])):			\
				(vassign_str(D[0], tmpchar),	\
				 AD0, 			\
				 S(D[0]))))

#define VADVARS(a,b)((tmpvar=b),				\
			 ((riggetd(a, 1, &i), i)?		\
			 	(assignvars(D[i], tmpvar),	\
			 	 (pristine=0),			\
				 D[i]):				\
				(assignvars(D[0], tmpvar),	\
				 TS(D[0]),			\
				 AD0, 			\
				 D[0])))

#define ADNUM(a,b)	((tmpnum=b),				\
			 ((i=a)?				\
				((getd(i=a, 1)->cur=INITNUM),	\
			 	 (pristine=0),			\
				 (D[i]->num=tmpnum),		\
				 tmpnum):			\
				((D[0]->num=tmpnum),		\
				 (D[0]->cur=INITNUM),		\
				 ToStr(D[0]),			\
				 AD0, 			\
				 tmpnum)))

#define ADSTR(a,b)	((tmpchar=b),				\
			 ((i=a)?				\
			 	(vassign_str(getd(i,1), tmpchar),\
			 	 (pristine=0),			\
				 S(D[i])):			\
				(vassign_str(D[0], tmpchar),	\
				 AD0, 			\
				 S(D[0]))))

#define ADVARS(a,b)((tmpvar=b),				\
			 ((i=a)?				\
			 	(assignvars(getd(i,1), tmpvar),	\
			 	 (pristine=0),			\
				 D[i]):				\
				(assignvars(D[0], tmpvar),	\
				 TS(D[0]),			\
				 AD0, 			\
				 D[0])))
