#include <stdio.h>
#include <math.h>
#include "ear.h"
#include "hash.h"
#include "awk.h"
#include "y.tab.h"
#include "header.h"
VARP	tmpvar;
void	END();
int	i;

VARP	AF_insert_commas_in_number();
char	tmpstr0[CONVSIZ];
VARP	tmpvar0;
VARP	funtmp0=(VARP) NULL;
extern fa	*reg1_5;
fa	*reg1_5;
VARP	Au_precision;
extern fa	*reg1_7;
fa	*reg1_7;
extern fa	*reg1_8;
fa	*reg1_8;
extern fa	*reg1_9;
fa	*reg1_9;
extern fa	*reg1_10;
fa	*reg1_10;
extern fa	*reg1_11;
fa	*reg1_11;
VAR	partmp0;
VAR	partmp1;
VAR	partmp2;
VAR	partmp3;

void
init_vars()
{
VARP	m_var();
HTBP	m_array();

VARP	tmpv;
reg1_5=mdfa(reparse("\\.[0-9]*"), 1);
Au_precision=m_var();
reg1_7=mdfa(reparse("..."), 1);
reg1_8=mdfa(reparse("[ ]+,"), 1);
reg1_9=mdfa(reparse(",[.]"), 1);
reg1_10=mdfa(reparse("-,"), 1);
reg1_11=mdfa(reparse(",$"), 1);
partmp0.string=m_str(1, "");
partmp1.string=m_str(1, "");
partmp2.string=m_str(1, "");
partmp3.string=m_str(1, "");
tmpvar0=m_var();
INPIPES=m_array();
OUTPIPES=m_array();
INFILES=m_array();
OUTFILES=m_array();
}

void
free_vars()
{
freefa(reg1_5);
free_var(Au_precision);
freefa(reg1_7);
freefa(reg1_8);
freefa(reg1_9);
freefa(reg1_10);
freefa(reg1_11);
free_str(partmp0.string);
free_str(partmp1.string);
free_str(partmp2.string);
free_str(partmp3.string);
free_var(tmpvar0);
free_array(INPIPES);
free_array(OUTPIPES);
free_array(INFILES);
free_array(OUTFILES);
}
extern int	upf[];

#define GETR	(rgetrec(D[0])?(splitrec(S(D[0]), upf),1):((*S(D[0])='\0'),NF=0,0))


#define AD0	(osplitrec(D[0], upf),(pristine=1),(D[0]->cur=INITSTR))
ad0() { AD0; }
void
assign_NF()
{
int	i;
int	tmpnum;
VARP	tmpvar;
0;
}
getfnr()
{
return (int) (FNR);
}
void
resetfnr()
{
0;
}
VARP
AF_insert_commas_in_number(Ap_x,Ap_num,Ap_fraction,Ap_integer)
VAR	Ap_x;
VAR	Ap_num;
VAR	Ap_fraction;
VAR	Ap_integer;
{
VARP	retval;
VARP funtmp0;
VAR partmp0;
VAR partmp1;
VAR partmp2;
VAR partmp3;
partmp0.string=m_str(1,"");
partmp1.string=m_str(1,"");
partmp2.string=m_str(1,"");
partmp3.string=m_str(1,"");
partmp0.num=0.0;
partmp1.num=0.0;
partmp2.num=0.0;
partmp3.num=0.0;
funtmp0=(VARP) NULL;
retval=m_var();
COPYSTR(&Ap_x);
COPYSTR(&Ap_num);
COPYSTR(&Ap_fraction);
COPYSTR(&Ap_integer);
ASTR((&Ap_num),(sresize(tmpvar0, 3096),sprintf(S(tmpvar0),"%19.6f",TONUM((&Ap_x))), S(tmpvar0)));
 print_var("x", &Ap_integer);
 print_var("num", &Ap_integer);
AVARS((&Ap_integer),(&Ap_num));
SUB(tmpvar0,reg1_5,"",(&Ap_integer),0);
pfp=stdout;
fputs(TS((&Ap_integer)),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
ASTR((&Ap_fraction),SUBSTR(tmpvar0,TS((&Ap_num)),(strlen(TS((&Ap_integer)))+1),(strlen(TS((&Ap_num)))-strlen(TS((&Ap_integer)))),0));
if ((docompN(Au_precision,0.0,NEQ,NOREV)))
{
FN(Au_precision),ASTR((&Ap_fraction),SUBSTR(tmpvar0,TS((&Ap_fraction)),1,((int)(Au_precision->num+1)),0));
}
else
{
ASTR((&Ap_fraction),"");
}
GSUB(tmpvar0,reg1_7,"&,",(&Ap_integer),0);
pfp=stdout;
fputs("y",pfp);
fputc(' ', pfp);
fputs(TS((&Ap_integer)),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
GSUB(tmpvar0,reg1_8,"",(&Ap_integer),0);
pfp=stdout;
fputs(TS((&Ap_integer)),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
SUB(tmpvar0,reg1_9,".",(&Ap_integer),0);
pfp=stdout;
fputs(TS((&Ap_integer)),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
SUB(tmpvar0,reg1_10,"-",(&Ap_integer),0);
pfp=stdout;
fputs(TS((&Ap_integer)),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
SUB(tmpvar0,reg1_11,"",(&Ap_integer),0);
pfp=stdout;
fputs("X",pfp);
fputc(' ', pfp);
fputs(TS((&Ap_integer)),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
AVARS((&Ap_num),(cat(tmpvar0,2,TS((&Ap_integer)), TS((&Ap_fraction)))));
pfp=stdout;
fputs(TS((&Ap_integer)),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
AVARS(retval,(&Ap_num));
FREESTR(Ap_x);
FREESTR(Ap_num);
FREESTR(Ap_fraction);
FREESTR(Ap_integer);
FREESTR(partmp0);
FREESTR(partmp1);
FREESTR(partmp2);
FREESTR(partmp3);
return retval;
;
FREESTR(partmp0);
FREESTR(partmp1);
FREESTR(partmp2);
FREESTR(partmp3);
FREESTR(Ap_x);
FREESTR(Ap_num);
FREESTR(Ap_fraction);
FREESTR(Ap_integer);
return retval;
}


main(argc, argv, envp)
int	argc;
char	*argv[],*envp[];
{
#ifdef KMALLOC
extern int Mt_trace;
if (argc>2 && !strncmp(argv[1], "-m", 2)) {
Mt_trace=2;
argc--; argv++;
}
#endif
mainloop(argc, argv, envp);
}

void
USR()
{
}

void
BEGIN()
{
{
TS((funtmp0=AF_insert_commas_in_number(VANUM((&partmp0),123456.0), (*nullvar), (*nullvar), (*nullvar)), funtmp0));
pfp=stdout;
fputs(TS(funtmp0),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
FREEFTM(funtmp0)
}
EXIT(1, 0);
}

void
END()
{
notseenend=0;
ADSTR(0,""); NF=0; assign_NF();
}
int	upf[]={-2};
