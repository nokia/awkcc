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

Acindex(str, ch)
char	*str;
register char	ch;
{
	register char	*s1;

	s1=str;
	for (; *s1 && *s1!=ch; s1++)
		;
	return (*s1?1+s1-str:0);
}

Ahindex(s, t)
char	s[];
register char	t[];
{
	register char	*s1, *s2;

	s1=s;
	s2=t;
	for (;*s2 && *s1++==*s2; s2++)
		;
	return (*s2?0:1);
}

Asindex(s, t)
char	*s;
register char	*t;
{
	register char	*s1, *s2, *s3;

	if (!*t) return 1;
	s1=s;
	while (*s1 && *s1!=*t)
		s1++;
	while (*s1) {
		while (*s1 && *s1!=*t)
			s1++;
		if (!*s1) break;
		s3=s1++;
		s2=t;
		for (;*s2 && *s3++==*s2; s2++)
			;
		if (!*s2)
			return (s1-s);
	}
	return (0);
}
