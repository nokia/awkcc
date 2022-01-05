
/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/
#include <stdio.h>
#include "awkcc.h"

extern int	currpush;

static int 	vvtab[7][7]=
	{
		/*YNUM  YINT  YSTR  YBTH  YSU   YNU   YUNK */

		{ YNUM, (-1), YSTR, (-1), (-1), (-1), YUNK },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ YSTR, (-1), YSTR, (-1), (-1), (-1), YSTR },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ YUNK, (-1), YSTR, (-1), (-1), (-1), YUNK }
	};

static int	eetab[7][7]=
	{
		{ YNUM, YNUM, YSTR, (-1), (-1), (-1), (-1) },
		{ YNUM, YINT, YSTR, (-1), (-1), (-1), (-1) },
		{ YSTR, YSTR, YSTR, (-1), (-1), (-1), YSTR },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ (-1), (-1), YSTR, (-1), (-1), (-1), (-1) }
	};

static int	evtab[7][7]=
	{	
		/* OPTVAR - EXPRESSION | */
		/*YNUM  YINT  YSTR  YBTH  YSU   YNU   YUNK */

		{ YNUM, YNUM, YSTR, (-1), (-1), (-1), YDEP },
		{ YNUM, YINT, YSTR, (-1), (-1), (-1), YDEP },
		{ YSTR, YSTR, YSTR, (-1), (-1), (-1), YSTR },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ (-1), (-1), (-1), (-1), (-1), (-1), (-1) },
		{ YDEP, YDEP, YSTR, (-1), (-1), (-1), (-1) }
	};

#define NOTYPEVAR(x)	(OPTVAR(x) ||			\
			 x==CALL ||			\
			 x==INDIRECT ||			\
			 x==ARRAY  ||			\
			 x==SYSARRAY||			\
			 x==ARRPARAM||			\
			 x==PARAMETER||			\
			 x==VARPARAM)

#define CANDO(x)	(NOTYPEVAR(x->t) ||		\
			 (ASSIGNOP(x->t) && x->y==YUNK))

nocando(e1, e2)
Nodep	e1, e2;
{
	if (!CANDO(e1)) if (!CANDO(e2)) return(1);
	return(0);
}

bothcando(e1, e2)
Nodep	e1, e2;
{
	if (CANDO(e1)) if (CANDO(e2)) return (1);
	return (0);
}

comparison(e)
Nodep	e;
{
	int	i;
	VARINFOP	left, right;
	int	l, r;
	Nodep	save;

	save=(Nodep) NULL;
	if (bothcando(e->LEFT, e->RIGHT)) {
		if (OPTVAR(e->LEFT->t)) {
			if (!(left=OVMEMQ(e->LEFT->s)))
				ER "No left compare" SX;
			l=(left->rt)[currpush];
		} else l=YUNK;
		if (OPTVAR(e->RIGHT->t)) {
			if (!(right=OVMEMQ(e->RIGHT->s)))
				ER "No right compare" SX;
			r=(right->rt)[currpush];
		} else r=YUNK;
		i=e->LEFT->w=e->RIGHT->w=vvtab[l][r];
		if (OPTVAR(e->RIGHT->t) && i!=YSTR)
			e->RIGHT->w=YBTH;
		if (OPTVAR(e->LEFT->t) && i!=YSTR)
			e->LEFT->w=YBTH;
		if (i==-1)
			ER "cmp error (%d and %d).\n", l, r SX;
 	} else if (nocando(e->LEFT, e->RIGHT)) {
		l=e->LEFT->y;
		r=e->RIGHT->y;
		i=e->LEFT->w=e->RIGHT->w=eetab[l][r];
	} else {
		/*  Must be a cando/!cando or vice-versa
		 */
		if (CANDO(e->RIGHT)) {
			save=e->RIGHT;
			e->RIGHT=e->LEFT;
			e->LEFT=save;
		}
		if (OPTVAR(e->LEFT->t)) {
			left=OVMEMQ(e->LEFT->s);
			l=(left->rt)[currpush];
		} else l=YUNK;
		r=e->RIGHT->y;
		i=evtab[l][r];
		switch(i) {
		case YDEP:
			/*  This, because docompN takes a double, not
			 *  an integer.
			 */
			if (OPTVAR(e->LEFT->t))
				e->LEFT->w=YBTH;
			else e->LEFT->w=YUNK;
			switch(e->RIGHT->y) {
			case YINT:
				e->RIGHT->w=YNUM;
				break;
			default:
				e->RIGHT->w=e->RIGHT->y;
			}
			break;
		case YNUM:
			if (e->RIGHT->y==YINT && (left->cur)[currpush]==YINT)
				e->LEFT->w=e->RIGHT->w=YINT;
			else e->LEFT->w=e->RIGHT->w=YNUM;
			break;
		case -1:
			ER "Err in comp (%d and %d).\n", l, r SX;
			break;
		default:
			e->LEFT->w=e->RIGHT->w=i;
		}
		if (save) {
			save=e->RIGHT;
			e->RIGHT=e->LEFT;
			e->LEFT=save;
		}
	}
}
