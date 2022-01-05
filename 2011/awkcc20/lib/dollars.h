/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

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

/* ASSIGNMENT */

#define VADNUM(a,b) (vadnum(a, b));

#define VADSTR(a,b) (vadstr(a,b))

#define VADVARS(a,b)(vadvars(a,b))

#define ADNUM(a,b)	(adnum(a,b))

#define ADSTR(a,b)	(adstr(a,b))

#define ADVARS(a,b) (advars(a,b))
