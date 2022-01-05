/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <stdio.h>
#include "ear.h"
#include "hash.h"

ELEMP	
delete(a, htb)
char	*a;
HTBP	htb;
{
	register unsigned int	j;
	register unsigned int	i; 
	register char	*s;
	register ELEMP  b; 
	ELEMP	newhead;

	/* a NULL return value has no particular meaning - it could be failure,
	 * or it could indicate that we have deleted the first element inthe bucket.
	 * (useful information for the "for..in" loops containing a delete).
	 */
	HASHFCN(a,s,i);
	j=i;
	i%=htb->len;
	if (b=htb->tab[i]) {
		if (b->hval==j && !strcmp(S(b), a)) {
			newhead=htb->tab[i]->next;
			free_elem(htb->tab[i]);
			htb->tab[i]=newhead;
			htb->num--;
			return((ELEMP) NULL);
		}
		for (; b->next; b=b->next)
			if (b->next->hval==j && !strcmp(S(b->next), a)) {
				newhead=b->next->next;
				free_elem(b->next);
				b->next=newhead;
				htb->num--;
				return(b);
			}
	}
	return((ELEMP) NULL);
}

VARP	
insert(inc, v, htb)
int 	inc;
VARP	v;
HTBP	htb;
{
	register unsigned int	j;
	register unsigned int	i; 
	register char	*s;
	register ELEMP	b; 
	ELEMP	save;
	char	*a;
	extern ELEMP	s_m_elem();

	if (inc) {
		/*  POST INCR/DECR INSERT */
		postmp(inc-1,v);
	} else {
		/*  V_INSERT */
		if (!STRAVAIL(v->cur)) 
			ToStr(v);
	}
	a=S(v);
	HASHFCN(a,s,i);
	j=i;
	i%=htb->len;
	GETLAST(htb,j,a);
	if (b=htb->tab[i]) {
		for (; b; b=b->next)
			if (b->hval==j && !strcmp(S(b), a)) {
				SAVELAST(htb, b);
				return(b->var);
			}
		save=htb->tab[i];
		htb->tab[i]=s_m_elem(v->string);
		htb->tab[i]->next=save;
	} else htb->tab[i]=s_m_elem(v->string);
	htb->num++;
	save=htb->tab[i];
	save->hval=j;
	if (htb->num>htb->next)
		resize(htb);
	SAVELAST(htb, save);
	return(save->var);
}

VARP	
member(a, htb)
char	*a;
HTBP	htb;
{
	register unsigned int	i;
	register unsigned int	j;
	register char	*s;
	register ELEMP	b;

	HASHFCN(a,s,i);	
	j=i;
	i%=htb->len;
	GETLAST(htb,j,a);
	if (b=htb->tab[i])
		for (; b; b=b->next)
			if (b->hval == j && !strcmp(S(b), a)) {
				SAVELAST(htb, b);
				return(b->var);
			}
	return((VARP) NULL);
}

VARP	
s_insert(a, htb)
char	*a;
HTBP	htb;
{
	register unsigned int	j;
	register unsigned int	i; 
	register char	*s;
	register ELEMP	b; 
	ELEMP	save;
	extern ELEMP	m_elem();

	HASHFCN(a,s,i);
	j=i;
	i%=htb->len;
	GETLAST(htb,j,a);
	if (b=htb->tab[i]) {
		for (; b; b=b->next)
			if (b->hval==j && !strcmp(S(b), a)) {
				SAVELAST(htb, b);
				return(b->var);
			}
		save=htb->tab[i];
		htb->tab[i]=m_elem(a);
		htb->tab[i]->next=save;
	} else htb->tab[i]=m_elem(a);
	htb->num++;
	save=htb->tab[i];
	save->hval=j;
	if (htb->num>htb->next)
		resize(htb);
	SAVELAST(htb, save);
	return(save->var);
}
VARP	
n_insert(n, htb)
long	n;
HTBP	htb;
{
	char	number[CONVSIZ];
	extern char	*numbers[];
	extern char	*OFMT;	/* new */

	if (HAVENUM(n))
		return (s_insert(numbers[n], htb));
	else {
		sprintf(number, OFMT, n);
		return(s_insert(number, htb));
	}
}

resize(htb)
register HTBP	htb;
{
	register ELEMP	tmp, save;
	register int	i;
	unsigned int	j;
	ELEMP	*m_buckets();
	register HTBP	tmptab;
	
	SP(tmptab=(HTBP)malloc(sizeof(HTB)), "resize");
	tmptab->len=htb->len*GROWTAB;
	tmptab->next=tmptab->len*FULLTAB;
	tmptab->tab=m_buckets(tmptab->len);
	for (i=0; i<htb->len; i++)
		if (tmp=htb->tab[i])
			while (tmp) {
				save=tmp->next;
				j=tmp->hval%tmptab->len;
				tmp->next=tmptab->tab[j];
				tmptab->tab[j]=tmp;
				tmp=save;
			}
	free(htb->tab);
	htb->tab=tmptab->tab;
	htb->len=tmptab->len;
	htb->next=tmptab->next;
	free(tmptab);
}

/* Basically bogus routine.  if 
 *
 *	for (i in j) delete i[j]
 *
 * then i should end up with some value.
 */

wclear_htb(htb, var)
HTBP	htb;
VARP	var;
{
	register ELEMP	tmp;
	register int	i;

	for (i=0; i<htb->len; i++) {
		if (htb->tab[i]) {
			tmp=htb->tab[i];
			R(tmp)++;
			free_str(var->string);
			var->string=tmp->string;
			clear_htb(htb);
			return;
		}
	}
}

clear_htb(htb)
HTBP	htb;
{
	register ELEMP	tmp;
	register int	i;
	register ELEMP	save;
	
	for (i=0; i<htb->len; i++) {
		if (htb->tab[i]) {
			tmp=htb->tab[i];
			while (tmp) {
				save=tmp->next;
				free_elem(tmp);
				tmp=save;
			}
			htb->tab[i]=(ELEMP) NULL;
		}
	}
	htb->num=0;
}

print_htb(htb)
HTBP	htb;
{
	ELEMP	tmp;
	int	i;
	
	fprintf(stderr, "len: %d, num: %d, index: %d.\n",
			 htb->len, htb->num, htb->index);
	for (i=0; i<htb->len; i++) {
		fprintf(stderr, "Bucket %d.\n", i);
		if (tmp=htb->tab[i]) {
			for (; tmp; tmp=tmp->next) {
				fprintf(stderr, "Element %d.\n", i);
  				fprintf(stderr, "%d(%s), %s.\n", 
				        i, S(tmp), S(tmp->var));
			}
		}
	}
	fprintf(stderr, "Done with this table.\n");
}
