/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/

isreg(s)
register char	*s;
{
	while (*s) {
		switch(*s++) {
		case '*':
		case '+':
		case '?':
		case '.':
		case '[':
		case ']':
		case '^':
		case '(':
		case ')':
		case '$':
		case '|':
			return 1;
		case '\\':
			if (s++)
				continue;
			return 0;
		}
	} 
	return 0;
}

fixreg(s)
register char	*s;
{
	register char	*t=s;

	for (;;) {
		switch(*s) {
		case '\0': *t='\0'; return;
		case '\\':
			switch(*(s+1)) {
			case '\\': *t++='\\'; s+=2; break;
			case 'n': *t++='\n'; s+=2; break;
			case 't': 
				*t++='\t'; s+=2; break;
			case 'f': *t++='\f'; s+=2; break;
			case 'r': *t++='\r'; s+=2; break;
			case 'b': *t++='\b'; s+=2; break;
			case '\0': *t='\0'; return;
			default: *t++= *(s+1); s+=2; break;
			}
			break;
		default: *t++=(*s++);
		}
	}
}
