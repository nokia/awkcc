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
	
extern char	errmsg[];
	
extern FILE	*in;
extern int	ARGC;
extern VARP	s_insert();
extern HTBP	Aa_ARGV;
extern VARP	Au_FILENAME;
extern int	ARGC;
extern VARP	vassign_str();

/* The first time round, just do initial assignments.
 * The fgets from /dev/null will then fail, causing /dev/null
 * to be closed and the next file opened.  Fine stuff.
 */

opennext(tf)
int	tf;
{
	static int	calledbefore;
	static int	argvpointer;
	char	tempstr[CONVSIZ];
	char	*strptr;
	int	j;

	if (!calledbefore)
		argvpointer=1;
	j=1;
	while (j && argvpointer<ARGC) {
		sprintf(tempstr, "%d", argvpointer);
		strptr=S(s_insert(tempstr, Aa_ARGV));
		if (!*strptr)
			argvpointer++;
   		else if (Acindex(strptr, '=')) {
			commandassign(strptr);
			argvpointer++;
		} else j=0;
	}
	if (!tf) return (1);
	if (in && in != stdin) {
		fclose(in);

		/* PSF 08/31/05 - make sure we don't keep reading when 
		 * last file is closed
		 */
		in=NULL;
	}

	/* Either we have some args to process, or
	 * we haven't called this before, so in<-stdin,
	 * or we have been processing a file which is now
	 * closed, and there are no more args to process,
	 * so return 0.
	 */
	if (argvpointer<ARGC) {
		calledbefore=1;
		sprintf(tempstr, "%d", argvpointer);
		strptr=S(s_insert(tempstr, Aa_ARGV));
		if (in=fopen(strptr, "r")) {
			ASTR(Au_FILENAME, strptr);
			argvpointer++;
			return (1);
		} else if (!strcmp(strptr, "-")) {
			ASTR(Au_FILENAME, strptr);
			in=stdin;
			argvpointer++;
			return(1);
		} else {
			sprintf(errmsg, "Can't open input file %s", strptr);
			awkerr(errmsg);
			/* LINT again
			 */
			return(0);
		}
	} else if (!calledbefore) {
		calledbefore=1;
		ASTR(Au_FILENAME, "-");
		in=stdin;
		return (1);
	}
	return (0);
}
