/***
 * Copyright 1987 by AT&T Bell Laboratories.
 * All rights reserved.
 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.
 ***/

#include <stdio.h>
#include <signal.h>
#include "ear.h"
#include "hash.h"
#include "awk.h"
#include "ctype.h"

/* hppa platform has RS in a header file */
#undef RS

static char SCCSID[]="@(#)awkrun.c	3.3\t4/23/90";

static VARP	tmpvar;
extern VARP	m_var();
extern void	usr();
extern void	init();
extern void	init_vars();
extern void	splitrec();
extern VARP	vassign_str();
	
char	errmsg[100];
	
/* -2 means that's all we need. 
 * -1 means get everything else
 * 0 means count NF, but don't insert
 */
	
extern int	upf[];
FILE	*files[MAXFILES];
/* One for saving real split, other for array default split */
char	ctest[MAX_CHARS];
char	sctest[MAX_CHARS];

STRP	nullstring;	
char	**environment;

char	*STRFS;	
fa	*REFS;
int	ND;
int	NF;
int	FNR;
int	NC;
int	pristine;
int	dflt;
int	rsdflt;
char	*OFS;
char	*RS;
char	*ORS;
char	*OFMT;
char	*SS;

VARP	gsubvar;
VARP	nullvar;
VARP	nullfld;
HTBP	nullarray;
VARP	*D;
HTBP	INPIPES;
HTBP	OUTPIPES;
HTBP	INFILES;
HTBP	OUTFILES;
HTBP	USRVARS;
VARP	Au_ARGC;
HTBP	Aa_ARGV;
VARP	Au_FILENAME;
VARP	Au_RLENGTH;
VARP	Au_RSTART;
VARP	Au_FS;
VARP	REALFS;
VARP	REALSPLIT;
VARP	Au_OFMT;
VARP	Au_SUBSEP;
VARP	Au_OFS;
VARP	Au_ORS;
VARP	Au_RS;
FILE	*pfp;
FILE	*in;
int notseenend;

int	ARGC;

mainloop(argc, argv, envp) 
int 	argc;
char	*argv[];
char	*envp[];
{
	void	BEGIN();
	void	USR();
	void	END();
	void fpecatch();

	signal(SIGFPE, fpecatch);
	environment=envp;

	OFS=" ";
	ORS="\n";
	in=fopen("/dev/null", "r");
	SP(D=(VARP *)malloc(sizeof(VARP)), "mainloop");
	(*D)=m_var();
	ND=1;
	dflt=1;		/* whitespace */
	rsdflt=0;	/* newline record separator */ 
	setctest('\t', sctest);
	setctest('\n', sctest);
	setctest(' ', sctest);
	setctest('\t', ctest);
	setctest('\n', ctest);
	setctest(' ', ctest);
	init_sysvars();
	init_vars();
	init_cmdline(argc, argv);
	opennext(0);
	pristine=1;
	notseenend=1;
	BEGIN();
	while (rgetrec(D[0])) {
		splitrec(S(D[0]), upf);
		USR();
	}
	END();
	EXIT(0, 0);
}

EXIT(real, estat)
int	real;
int	estat;
{
	/*  Fix such that it closes all files first?
	 */
	int	i;
	static int	restat;

	switch (real) {
	case 1:
		restat=estat;
		break;
	case 2:
		restat=estat;
		return;
	case 0:
		break;
	default:
		ER "INTERNAL: bad arg %d for exit", real SX;
		break;
	}
	closefiles(OUTPIPES);
	closefiles(OUTFILES);
#define FREEALL
#ifdef FREEALL
	for (i=0; i<ND; i++)
		free_var(D[i]);
	free(D);
	free_var(nullvar);
	free_var(gsubvar);
	free_var(Au_OFMT);
	free_var(Au_SUBSEP);
	free_var(Au_ARGC);
	free_var(Au_FILENAME);
	free_var(Au_ORS);
	free_var(Au_OFS);
	free_array(Aa_ARGV);
	free_array(nullarray);
	free_var(Au_FS);
	free_var(REALFS);
	free_var(REALSPLIT);
	free_var(Au_RS);
	free_var(Au_RSTART);
	free_var(Au_RLENGTH);
	/* free_vars() must come before USRVARS because it touches a few things
	 * up
	 */
	free_vars();
	free_array(USRVARS);
	nullstring->refcnt=0;	/* was ++'d to make it undeletable */
	free_str(nullstring);
	R(nullfld)--;
	free_var(nullfld);
	cleanfas();
	if (REFS)
		freefa(REFS);
#endif
	exit(restat); 
}

commandassign(str)
char	*str;
{
	register int	i;
	char	temporary[100];
	extern char	*qstring();

	for (i=0; str[i]!='='; i++) {
		if (i>=99)
			awkerr("commandassign can't handle vars of length>100");
		temporary[i]=str[i];
	}
	temporary[i]='\0';
	doassign(temporary, qstring(str+i+1, '\0'));
}

doassign(s1, s2)
char	*s1;
char	*s2;
{
	VARP	tmp, member();

	if (tmp=member(s1, USRVARS)) {
		if (tmp->num == 3.0) {
			ANAS(*((VARP *)S(tmp)),s2); 
		} else if (tmp->num == 1.0) {
			*((double *)S(tmp))=atof(s2);
		} else if (tmp->num == 2.0) {
			*((int *)S(tmp))=(int)atof(s2);
		}
	} else if (!strcmp(s1, "FS")) {
		AFSSTR(Au_FS, s2);
		TN(Au_FS);
	} else if (!strcmp(s1, "OFMT")) {
		AOFMTSTR(Au_OFMT, s2);
		TN(Au_OFMT);
	} else if (!strcmp(s1, "OFS")) {
		AOFSSTR(Au_OFS, s2);
		TN(Au_OFMT);
	} else if (!strcmp(s1, "ORS")) {
		AORSSTR(Au_ORS, s2);
		TN(Au_ORS);
	} else if (!strcmp(s1, "ARGC")) {
		AARGCSTR(Au_ARGC, s2);
		TN(Au_ARGC);
	} else if (!strcmp(s1, "RS")) {
		ARSSTR(Au_RS, s2);
		TN(Au_RS);
	} else if (!strcmp(s1, "NR") 		||
		   !strcmp(s1, "FILENAME") 	||
		   !strcmp(s1, "FNR") 		||
		   !strcmp(s1, "NF")) {
		/* do nothing, since it will be overriden anyway */
	} else if (!strcmp(s1, "RSTART")) {
		ANAS(Au_RSTART, s2);
	} else if (!strcmp(s1, "RLENGTH")) {
		ANAS(Au_RLENGTH, s2);
	} else if (!strcmp(s1, "SUBSEP")) {
		ASUBSEPSTR(Au_SUBSEP, s2);
		TN(Au_SUBSEP);
	} else ER "Illegal command-line assignment: compile with `-u %s' option\n\tor, put `#awkcchint -u %s' in your script", s1, s1 SX;
}

flushfiles(array)
HTBP	array;
{
	int	j1;
	int	l1;
	ELEMP	elem1;
	ELEMP	bkt1;

	j1=0;
	elem1=((ELEMP) NULL);
	l1=array->num;
	for (;j1<array->len && !(bkt1=array->tab[j1]); j1++)
		;
	for (;;) {
		if (!l1)
			break;
		else if (bkt1) {
			l1--;
			elem1=bkt1;
		} 
		else {
			for (j1++; j1<array->len && !(array->tab[j1]); j1++)
				;
			bkt1=(array->tab[j1]);
			if (j1<array->len) {
				l1--;
				elem1=bkt1;
			}
			else break;
		}
		fflush(files[(int)elem1->var->num]);
		bkt1=bkt1->next;
	}
}

closefiles(array)
HTBP	array;
{
	int	j1;
	int	l1;
	ELEMP	elem1;
	ELEMP	bkt1;

	j1=0;
	elem1=((ELEMP) NULL);
	l1=array->num;
	for (;j1<array->len && !(bkt1=array->tab[j1]); j1++)
		;
	for (;;) {
		if (!l1)
			break;
		else if (bkt1) {
			l1--;
			elem1=bkt1;
		} 
		else {
			for (j1++; j1<array->len && !(array->tab[j1]); j1++)
				;
			bkt1=(array->tab[j1]);
			if (j1<array->len) {
				l1--;
				elem1=bkt1;
			}
			else break;
		}
		filedrop(S(elem1));
		bkt1=bkt1->next;
	}
}

init_sysvars()
{
	extern HTBP	m_array();
	extern VARP	s_insert();
	extern VARP	m_var();
	extern STRP	m_str();

	nullstring=m_str(1, "");
	nullstring->refcnt++;	/* insure that it is never junked */
	nullvar=m_var();
	gsubvar=m_var();
	nullfld=m_var(); S(nullfld)[0]='\0'; nullfld->cur=(NUMFLD|DIDFLD|STRFLD); R(nullfld)++;
	Au_OFMT=m_var();
	Au_SUBSEP=m_var();
	Au_ARGC=m_var();
	Au_FILENAME=m_var();
	vassign_str(Au_FILENAME, "-");
	Au_ORS=m_var();
	Au_OFS=m_var();
	Aa_ARGV=m_array();
	nullarray=m_array();
	Au_FS=m_var();
	REALFS=m_var();
	REALSPLIT=m_var();
	Au_RS=m_var();
	Au_RSTART=m_var();
	Au_RLENGTH=m_var();
	ASTR(Au_FS, " ");
	STRFS=" ";
	ASTR(Au_RS, "\n");
	AOFMTSTR(Au_OFMT, "%.6g");
	ASUBSEPSTR(Au_SUBSEP, "\034");
	USRVARS=m_array();
}

init_cmdline(argc, argv)
register int	argc;
register char	*argv[];
{
	register int	j;
	extern HTBP	m_array();
	extern VARP	s_insert();
	extern VARP	m_var();
	VARP	tmp;
	char	subscript[CONVSIZ];
	int	foundfile;
	extern fa	*mdfa();

	tmp=s_insert("0", Aa_ARGV);
	ASTR(tmp, argv[0]);
	argc--;
	argv++;
	if (*argv && !(strncmp(*argv, "-F", 2))) {
		if (argv[0][2]) {
			fixreg(&argv[0][2]);
			ASTR(Au_FS, &argv[0][2]);
			clearctest(argv[0][2]=='t'?'\t':argv[0][2]);
			ASTR(REALFS, S(Au_FS));
			STRFS=S(REALFS);
			if (strlen(STRFS)>1) {
				if (isreg(STRFS)) {
					dflt=3;
					if (REFS)
						freefa(REFS);
					REFS=mdfa(reparse(STRFS), 1);
				} else {
					dflt=2;
					fixreg(STRFS);
				}
			} else dflt=0;
			argv++;
			argc--;
		} else {
			if (argc<=1)
				ER "No '-F' followup" SX;
			fixreg(argv[1]);
			ASTR(Au_FS, argv[1]);
			clearctest(argv[1][0]=='t'?'\t':argv[1][0]);
			ASTR(REALFS, S(Au_FS));
			STRFS=S(REALFS);
			if (strlen(STRFS)>1) {
				if (isreg(STRFS)) {
					if (REFS)
						freefa(REFS);
					REFS=mdfa(reparse(STRFS), 1);
					dflt=3;
				} else {
					dflt=2;
					fixreg(STRFS);
				}
			} else dflt=0;
			argv++;
			argc--;
			argv++;
			argc--;
		}
	}
	ARGC=1;
	foundfile=0;
	for (j=0; j<argc; j++) {
		if (!(Acindex(argv[j], '=') || foundfile)) {
			ASTR(Au_FILENAME, argv[j]);
			foundfile=1;
		}
		if (foundfile) {
			sprintf(subscript, "%d", ARGC++);
			tmp=s_insert(subscript, Aa_ARGV);
			ASTR(tmp, argv[j]);
			tmp->cur=INITSTR;
		} else if (Acindex(argv[j], '='))
			commandassign(argv[j]);
	}
	sprintf(subscript, "%d", ARGC);
	ANAS(Au_ARGC, subscript);
}


char *numbers[] = {
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"11",
	"12",
	"13",
	"14",
	"15",
	"16",
	"17",
	"18",
	"19",
	"20",
	"21",
	"22",
	"23",
	"24",
	"25",
	"26",
	"27",
	"28",
	"29",
	"30",
	"31",
	"32",
	"33",
	"34",
	"35",
	"36",
	"37",
	"38",
	"39",
	"40",
	"41",
	"42",
	"43",
	"44",
	"45",
	"46",
	"47",
	"48",
	"49",
	"50",
	"51",
	"52",
	"53",
	"54",
	"55",
	"56",
	"57",
	"58",
	"59",
	"60",
	"61",
	"62",
	"63",
	"64",
	"65",
	"66",
	"67",
	"68",
	"69",
	"70",
	"71",
	"72",
	"73",
	"74",
	"75",
	"76",
	"77",
	"78",
	"79",
	"80",
	"81",
	"82",
	"83",
	"84",
	"85",
	"86",
	"87",
	"88",
	"89",
	"90",
	"91",
	"92",
	"93",
	"94",
	"95",
	"96",
	"97",
	"98",
	"99",
	"100",
	"101",
	"102",
	"103",
	"104",
	"105",
	"106",
	"107",
	"108",
	"109",
	"110",
	"111",
	"112",
	"113",
	"114",
	"115",
	"116",
	"117",
	"118",
	"119",
	"120",
	"121",
	"122",
	"123",
	"124",
	"125",
	"126",
	"127",
	"128",
	"129",
	"130",
	"131",
	"132",
	"133",
	"134",
	"135",
	"136",
	"137",
	"138",
	"139",
	"140",
	"141",
	"142",
	"143",
	"144",
	"145",
	"146",
	"147",
	"148",
	"149",
	"150",
	"151",
	"152",
	"153",
	"154",
	"155",
	"156",
	"157",
	"158",
	"159",
	"160",
	"161",
	"162",
	"163",
	"164",
	"165",
	"166",
	"167",
	"168",
	"169",
	"170",
	"171",
	"172",
	"173",
	"174",
	"175",
	"176",
	"177",
	"178",
	"179",
	"180",
	"181",
	"182",
	"183",
	"184",
	"185",
	"186",
	"187",
	"188",
	"189",
	"190",
	"191",
	"192",
	"193",
	"194",
	"195",
	"196",
	"197",
	"198",
	"199",
	"200",
	"201",
	"202",
	"203",
	"204",
	"205",
	"206",
	"207",
	"208",
	"209",
	"210",
	"211",
	"212",
	"213",
	"214",
	"215",
	"216",
	"217",
	"218",
	"219",
	"220",
	"221",
	"222",
	"223",
	"224",
	"225",
	"226",
	"227",
	"228",
	"229",
	"230",
	"231",
	"232",
	"233",
	"234",
	"235",
	"236",
	"237",
	"238",
	"239",
	"240",
	"241",
	"242",
	"243",
	"244",
	"245",
	"246",
	"247",
	"248",
	"249",
	"250",
	"251",
	"252",
	"253",
	"254",
	"255",
	"256",
	"257",
	"258",
	"259",
	"260",
	"261",
	"262",
	"263",
	"264",
	"265",
	"266",
	"267",
	"268",
	"269",
	"270",
	"271",
	"272",
	"273",
	"274",
	"275",
	"276",
	"277",
	"278",
	"279",
	"280",
	"281",
	"282",
	"283",
	"284",
	"285",
	"286",
	"287",
	"288",
	"289",
	"290",
	"291",
	"292",
	"293",
	"294",
	"295",
	"296",
	"297",
	"298",
	"299"
};

void
fpecatch()
{
	ER "floating point exception (division by zero)" SX;
}

coredump()
{
	int	*i;

	i=(int *) NULL;
	*i=1;
}

char *qstring(s, delim)	/* collect string up to delim */
	char *s;
	int delim;
{
	char *q;
	int c, n;
	static char	cbuf[512];
	int CBUFLEN=512;

	for (q = cbuf; (c = *s) != delim; s++) {
		if (q >= cbuf + CBUFLEN - 1)
			awkerr("string %.10s... too long", cbuf);
/* an artifact from awk - there's no reason the newline should be illegal...
		else if (c == '\n')
			awkerr("newline in string %.10s...", cbuf);
*/
		else if (c != '\\')
			*q++ = c;
		else	/* \something */	
			switch (c = *++s) {
			case '\\':	*q++ = '\\'; break;
			case 'n':	*q++ = '\n'; break;
			case 't':	*q++ = '\t'; break;
			case 'b':	*q++ = '\b'; break;
			case 'f':	*q++ = '\f'; break;
			case 'r':	*q++ = '\r'; break;
			default:
				if (!isdigit(c)) {
					*q++ = c;
					break;
				}
				n = c - '0';
				if (isdigit(s[1])) {
					n = 8 * n + *++s - '0';
					if (isdigit(s[1]))
						n = 8 * n + *++s - '0';
				}
				*q++ = n;
				break;
			}
	}
	*q = '\0';
	return cbuf;
}
