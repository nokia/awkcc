# awkcc

awkcc is an awk-to-C transcompiler. It was originally implemented by Chris Ramming,
then at Bell Labs, circa 1988.

The intent of this repo is to make the source code of awkcc public for historical
purposes. This code is not being further developed or maintained.

## Repo structure

The repo contains two snapshots of the source code in the [1988](1988) and
[2011](2011) folders. Both snapshots contain the complete codebase of awkcc
at the given time. The codebase includes the original documentation.  Much
thanks to Arnold Robbins for his help in recovering these snapshots.

## Build Instructions Courtesy of Jeff Paranich

These instructions apply to the 2011 version of the code.

### Requirements

Yacc is required to  build.  On a Debian-derived system with `apt`,
install it with `apt install bison`.

### Compilation

#### Build in Linux
Build: `make awkcc TYPE=darwin CC=gcc`

Clean: `make force`

#### Build in MinGW (MSys)
Build: `make awkcc TYPE=cygwin CC=gcc CFLAGS="-D__CYGWIN__ -DWIN_NUL_DEV"`

Clean: `make force`

It is neccessary to define __CYGWIN__ or else the compiler will compile
a different implementation of the function awkerr in awkerr.c, which
relies on `_doprnt`, which is non-portable (no POSIX, no ANSI/ISO C).

See the Makefile for other build type parameters (solaris, hppa).

After building, `make` will create three files worth noting:

* The executable 'awkcc' in awkcc20/awkcc
* The script 'awkcc.sh' in awkcc20/awkcc 
* The library 'libAWK.a' in awkcc20/lib

You will need to edit awkcc.sh HEADERDIR and AWKLIB variables (at the top
of the script) to where you ultimately install libAWK.a and the header
files on your system.  You only need to install the headers in ./include
({awk,dollars,ear,hash,header,specassign,y.tab}.h)

### Usage

    awkcc -o nwc -c nwc.c nwc.awk

awkcc will automatically call awkcc.sh, a shell script for compiling
the resulting .c file.  It may require hand-tuning based on your include
and library paths, but is pretty straight forward.

Alternatively if want to compile the generated .c manually you can do so with:

    gcc -I../include -o nwc nwc.c libAWK.a -lm

Note the order of statements matter; libAWK.a must come after `*.c`.

### Minor Commit Changes to Original Bell Labs Source

These were the minor changes in the source to allow it to compile
(for those interested).

#### [Added to awkrun.c]

To allow execution in Windows through MinGW,
it is neccessary to have an alternate to /dev/null:

	#ifdef WIN_NUL_DEV
	in=fopen("nul", "r");
	if(in==NULL)
		fprintf(stderr,"Could not open nul for reading.\n");
	#else
	in=fopen("/dev/null", "r");
	if(in==NULL)
		fprintf(stderr,"Could not open /dev/null for reading.\n");	
	#endif


#### [Changed in main.c]

    Nodep yyparse();
     -to-
    int yyparse();

## License

awkcc is licensed under the [BSD 2-Clause "Simplified" License](LICENSE).
