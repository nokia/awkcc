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


void
init_vars()
{
VARP	m_var();
HTBP	m_array();

VARP	tmpv;
INPIPES=m_array();
OUTPIPES=m_array();
INFILES=m_array();
OUTFILES=m_array();
}

void
free_vars()
{
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
{
AFSSTR(Au_FS,"v");
}
{
pfp=stdout;
fputs(GETDSTR(0,1),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
pfp=stdout;
fputs(GETDSTR(0,2),pfp);
fputc('\n', pfp)
;
if (ferror(pfp)) awkperr();
}
}

void
BEGIN()
{
{
AFSSTR(Au_FS,S(Au_FS));
}
}

void
END()
{
notseenend=0;
ADSTR(0,""); NF=0; assign_NF();
}
int	upf[]={1, 2, -2};
