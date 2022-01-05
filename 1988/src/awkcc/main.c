
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

static char SCCSID[]="@(#)main.c	3.1\t7/11/88";

VARINFOP	*symtab;
uchar		*cmdname;
uchar		*infilename;
int		hasSUBSEP, hasOFMT, hasOFS, hasORS;
int		illum = 1;
int		dbg;
int		OFSbug;
int		verbose=0;
int		awkdbg=0;
int		changedollar=0;
int		errorflag=0;
int		bufferflag=0;
int		pflag=0;
int		typeflag=0;
int		joption=0;
char		*objfile;
char		*cfile;
uchar		*lexprog;
Nodep	winner;
extern FILE	*yyin; 
int compile_time=1;
char	errbuf[200];

main(argc, argv)
int	argc;
uchar	**argv;
{
	int	c; 
	extern char	*optarg;
	extern int	optind;
	int	dupnew;
	Nodep	yyparse();
	char	cmd[300];

	cmdname=argv[0];
	add_sym("");
	cfile="awker.c";
	objfile="a.out";
	while ((c=getopt(argc, argv, "BIJUVPDdbu:c:o:")) != EOF)
		process_args(c, optarg, 0);
	if (argv[optind+1] || !argv[optind])
		ER "Illegal invocation - too many/few arguments" SX;
	infilename=(uchar *)argv[optind];
	if ((yyin=fopen(infilename, "r"))==NULL)
		ER "Can't awk source file \"%s\".", infilename SX;
	yyparse();
	compile_time=0;
	arraytype();
	dupnew=dup(1);
	close(1);
	if (fopen(cfile, "w")==NULL)
		ER "Illegal C output file" SX;
	if (dbg) setbuf(stdout, NULL); 
	addsysvars(winner);
	if (verbose || dbg) print_symtab();
	if (dbg) printnode(winner, 0);
	follow(winner);
	if (!typeflag) pretype(symtab);
	if (dbg) printnode(winner, 0);
	makeprog(winner, symtab);
	fclose(yyin);
	fclose(stdout);
	dup(dupnew);
#ifdef EXPTOOLS
	sprintf(cmd, "$TOOLS/lib/awkcc/awkcc.sh %s %s %d", 
#else
	sprintf(cmd, "awkcc.sh %s %s %d", 
#endif
			cfile, 
			objfile, 
			(pflag?1:(joption?2:0)));
	system(cmd);
	exit(0);
}

process_args(c, s, v)
char	c;
char	*s;
int	v;
{
	switch(c) {
	case 'I':
		illum=0;
		break;
	case 'J':
		joption=1;
		break;
	case 'd':
		fprintf(stderr, "Sorry, no debugger.\n");
		break;
	case 'V':
		verbose=1;
		break;
	case 'P':
		pflag=1;
		break;
	case 'D':
		dbg=1;
		break;
	case 'c':
		if (v && strcmp(cfile, "awker.c")) {
			fprintf(stderr, "WARNING: `-c %s' not overriden.\n",cfile);
			break;
		}
		cfile=s;
		break;
	case 'u':
		add_uvar(s);
		break;
	case 'o':
		if (v && strcmp(objfile, "a.out")) {
			fprintf(stderr, "WARNING: `-o %s' not overriden.\n",objfile);
			break;
		}
		objfile=s;
		break;
	case 'U':
		typeflag=1;
		break;
	case 'B':
		bufferflag=0;
		break;
	case 'b':
		bufferflag=1;
		break;
	case '?':
	default:
		ER "Illegal option %c", (char) c SX;
	}
}
