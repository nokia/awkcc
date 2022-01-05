/*
Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
	All Rights Reserved

Note: This software was created by the Bell Laboratories unit of AT&T.
Bell Laboratories was subsequently part of Lucent Technologies, later part of
Alcatel-Lucent, and now part of Nokia; some copyrights may have been assigned
by AT&T to its successors. This license is granted by Nokia solely to the
extent of its right to do so.
*/


extern ELEMP	delete();
extern HTBP	m_array();
extern FILE	*fileget();
extern FILE	*pipeget();
extern double	awkmod();
extern double	atof();
extern void	ToStr();
extern char	*Asubstr();
extern char	*sresize();
extern char	*premp();
extern char	*postmp();
extern char	*xprintf();
extern VARP	assignvars();
extern VARP	cat();
extern VARP	Atocase();
extern VARP	m_var();
extern VARP	vassign_str();
extern VARP	getd();
extern VARP	riggetd();
extern VARP	insert();
extern VARP	s_insert();
extern VARP	n_insert();
extern VARP	member();
extern fa	*ins_regexp();
extern fa	**regs;
extern char	*patbeg;
extern int	patlen;
extern VARP	*D;
extern VARP	Au_ARGC;
extern VARP	Au_FILENAME;
extern VARP	Au_RLENGTH;
extern VARP	Au_RSTART;
extern VARP	Au_FS;
extern VARP	REALFS;
extern VARP	Au_OFMT;
extern VARP	Au_SUBSEP;
extern VARP	Au_OFS;
extern VARP	Au_ORS;
extern VARP	Au_RS;
extern HTBP	Aa_ARGV;
extern HTBP	USRVARS;
extern HTBP	nullarray;
extern HTBP	REGEXPS, REGEXPS1;
extern HTBP	INPIPES, OUTPIPES;
extern HTBP	INFILES, OUTFILES;
extern char	*OFMT;
extern char	*SS;
extern VARP	mD0();
extern int	ND, NF, NC, FNR, ARGC;
extern int	pristine;
extern int	dflt, rsdflt;
extern char	IFS;
extern char	*RS;
extern char	*STRFS;
extern char	*OFS;
extern char	*ORS;
extern char	ctest[];
extern fa	*REFS;
extern VARP	nullvar;
char		*tmpchar;
extern int	notseenend;
extern FILE	*pfp;
