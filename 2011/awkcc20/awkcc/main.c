/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <stdio.h>
#include "awkcc.h"

static char SCCSID[]="@(#)main.c	3.1\t7/11/88";

VARINFOP	*symtab;
char		*cmdname;
char		*infilename;
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
int		utssw=1;
char		*ccparm;
char		*objfile;
char		*cfile;
uchar		*lexprog;
Nodep	winner;
extern FILE	*yyin,*yyout; 
int compile_time=1;
char	errbuf[200];

main(argc, argv)
int	argc;
char	**argv;
{
	int	c; 
	extern char	*optarg;
	extern int	optind;
	int	dupnew;
	Nodep	yyparse();
	char	cmd[300];

	yyin=stdin;
	yyout=stdout;

	cmdname=argv[0];
	add_sym("");
	cfile="awker.c";
	objfile="a.out";
	ccparm="\" \"";
#ifdef	UTS
	utssw=0;
#endif
	while ((c=getopt_(argc, argv, "BIJUVPDdbu:c:o:f:")) != EOF)
		process_args(c, optarg, 0);
	if (argv[optind+1] || !argv[optind])
		ER "Illegal invocation - too many/few arguments" SX;
	infilename=argv[optind];
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
	sprintf(cmd, "awkcc.sh %s %s %s %d", 
			cfile, 
			objfile, 
			ccparm,
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
	case 'f':
		if ((utssw == 0) && (strncmp(s, "-TR", 3) == 0)) {
			ccparm="";
			strcat(strcpy(ccparm, "\""), s);
			strcat(ccparm, "\"");
			break;
		} else {
			fprintf(stderr, "WARNING: `-f %s' not used - Non-UTS/Non -TR flag detected.\n",s);
			ccparm="\" \"";
			break;
		}
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
