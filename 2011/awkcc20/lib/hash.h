/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

/* Must be included after ear.h
 */

#define FULLTAB	2
#define GROWTAB 4
#define STARTAB	12

typedef struct elemp {
	STRP	string;
	VARP	var;
	unsigned int	hval;
	struct elemp	*next;
} ELEM, *ELEMP;

typedef struct {
	char	*laststr;	/*  pointer to last accessing string */
	ELEMP	lastelem;	/*  pointer to last accessed element */
	int	index;		/*  index into the primelist 	*/
	int	len;		/*  number of buckets 		*/
	int	num;		/*  number of elements		*/
	int	next;		/*  resize breakpoint		*/
	ELEMP	*tab;		/*  array of buckets		*/
} HTB, *HTBP;

#define HASHFCN(str,tmp,accum)  for (accum=0,tmp=str;*tmp;tmp++) \
					accum=(*tmp+31*accum);

#ifdef LASTELEMENT
#	define GETLAST(h,hv,s)	if (h->lastelem && \
				   h->lastelem->hval==hv && \
				   !strcmp(S(h->lastelem),s)) {\
					return(h->lastelem->var); \
				}
#	define SAVELAST(h,v)	h->lastelem=v
#else
#	define SAVELAST(h,v)
#	define GETLAST(h,hv,s)
#endif
