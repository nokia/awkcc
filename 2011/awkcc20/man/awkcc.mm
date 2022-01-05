.PM BP
.ND  "February 29, 1988"
.TL  59547-880229-01 40853
\fI\fIawkcc\fP - an \fIawk\fP to C Translator
.AU "J. Christopher Ramming" jcr MH 59547 5954 30H-003
.MT "TECHNICAL MEMORANDUM"

.ds HP +2 +2 +1 +1
.ps 10
.HU "Introduction"
.P
\fIAwk\fP has been described as "a convenient and expressive
programming language that can be applied to a wide variety
of computing and data-manipulation tasks."\*(Rf
.RS
Aho, Weinberger, Kernighan:  The AWK Programming Language. Addison-Wesley 1988.
.RF
Problems for which \fIawk\fP is well-suited can often be solved in just
a few lines of code, and it is sufficiently flexible that many
have used it for rapid prototyping with intent to produce a final
hand-coded system in C\*(Rf.
.RS
B.J. Beare: "Conversion of AWK Scripts to C Code Made Simple", case 40094-21.
.RF
.P
\fIAwkcc\fP is a preprocessor that translates \fIawk\fP code into C, 
which is subsequently compiled using \fIcc\fP.  This tool
can multiply the speed of many \fIawk\fP scripts, and increases of
a factor between 2 and 6 are common.  \fIAwkcc\fP has been designed
with transparency in mind;  that is, scripts should produce the same
results after compilation as they do with the interpreter.  In addition,
since \fIawkcc\fP uses C as its target language, the package is
easily ported to any UNIX 
.FS " "
UNIX\u\s-1\(rg\s+1\d is a registered trademark of AT&T.
.FE
environment, and has been used on a range
of machines and operating systems including System V, BSD4.3, and
Sun 3.2.
.P
This memo will give a brief discussion of \fIawk\fP and then describe 
\fIawkcc\fP, an \fIawk\fP-to-C translator.  It explains what \fIawkcc\fP is and
how to invoke it, discusses situations in which it makes \fIawk\fP
a more useful tool, and gives a high-level overview of its construction.  
Finally, it addresses issues of transparency, portability, and availability.
.H 1 "About \fIawk\fP"
.H 2 "\fIAwk\fP is a good thing."
.P
\fIAwk\fP is a terse language.  It
has high level constructs -  associative arrays, a pattern/action
control structure, and convenient notation for field variable -
that make for short but powerful programs.  It has the advantage
of an action language that looks much like C but which eliminates
the need for troublesome details like variable declaration and
initialization.  It treats expressions as numeric or string depending on
context, permitting the user to omit type information, and it
performs implicit computation like record splitting and record
counting on behalf of the programmer\*(Rf.
.RS
Aho, Weinberger, Kernighan:  "AWK - A Pattern Scanning and Processing Language", 11272-850619-06TM
.RF
.P
These features make possible shorter, simpler programs than
could be written in lower-level languages.  Although programmer productivity 
is difficult to measure, certain groups have found that 
by using \fIawk\fP rather than C they have been able to complete 
applications in a tenth the time\*(Rf.
.RS
H.S. Moscovitz: "Quarterly Status Report - Methodology Group Aug-Oct 1987"
.RF
.H 2 "There is no free lunch"
.P
However, as a rule there is a tradeoff between the the rate at
which programs can be developed and the rate at which they execute.
Precision is sometimes forfeit when using \fIawk\fP, and its most
powerful features can actually be sources of relative inefficiency.
.P
For instance, high-level features like associative arrays are
free in \fIawk\fP but take C programmers quite a while implement.
But \fIawk\fP programmers commonly use this construct to implement
ordinary
numerically-subscripted arrays, whereas C programmers have a much
less expensive alternative.  Only when the full functionality of 
associative arrays is necessary can they be used with nearly optimal
performance results.
.P
\fIAwk\fP also trades notational convenience for performance.
Specifically, the omission of declarations and initializations, and the
typeless nature of its expressions each have a cost associated
with them.
.P
Lastly, \fIawk\fP has heretofore always been invoked as an interpreter,
making an admittedly tedious compilation step unnecessary.  This
is another costly convenience, for each invocation then requires a parsing 
of the program.  Also, walking the program graph to choose appropriate 
actions is an unnecessary overhead, and certain optimizations
are neglected because detecting them takes too much time.
.P
\fIAwkcc\fP is an \fIawk\fP-to-C translator.  It makes \fIawk\fP more useful
by addressing and eliminating some of the interpreter's inherent 
inefficiencies.  It has value in that if it succeeds in improving 
programs' execution
times, applications that were previously too slow when written in
\fIawk\fP may no longer require re-coding in C by hand.
.H 1 \fIAwkcc\fP
.H 2 Description
.H 3 "Input."
.P
In order to use \fIawkcc\fP an \fIawk\fP program must be placed in
in a file:
.DS	
	$ ls
	nwc.awk
	$ cat nwc.awk
	{
		words += NF;
		chars+=1+length($0)
	}
	END { print FILENAME ":", NR, words, chars }
	$
.DE
.H 3 "Invocation."
.P
Given that one has produced an \fIawk\fP program in some file, then
to compile it one would invoke \fIawkcc\fP with that file as an
argument:
.DS
	$ awkcc nwc.awk
	$ ls -F
	a.out*
	awker.c
	nwc.awk
	$
.DE
If compilation completes without errors you will find that two new 
files have been created, called \fIawker.c\fP and \fIa.out\fP, 
respectively.  \fIAwker.c\fP is a file created by \fIawk\fP
which contains C code corresponding to the \fIawk\fP program.
\fIAwkcc\fP also created a file \fIa.out\fP, which is
the result of invoking \fIcc\fP on \fIawker.c\fP and linking the object
file with a library of routines the program may need.
\fIa.out\fP is executable, and can be invoked in the same way as one
would invoke the \fIawk\fP program using the interpreter.  That is
to say, the following are equivalent:
.DS
	$ \fIawk\fP -f nwc.awk /etc/passwd
	/etc/passwd: 311 1749 27840
	$ a.out /etc/passwd
	/etc/passwd: 311 1749 27840
	$
.DE
The \fIa.out\fP accepts any arguments \fIawk\fP accepts with two exceptions:
the "-f program" argument no longer makes sense, and if one intends
to use command line assignment then it is necessary to prepare for this
possibility with a special compilation flag.
.H 3 Options.
.P
Options to the translator are: (see Appendix A)
.BL
.LI
\fI-u var\fP:
This options specifies that \fIvar\fP may be assigned 
on the command line;  ordinarily this would be forbidden
in order to permit certain optimizations
on \fIvar\fP that cannot be done if  its value might
be arbitrarily affected by the command line.
.LI
\fI-o outfile\fP:
This option specifies that the filename \fIoutfile\fP be
used instead of the default \fIa.out\fP.
.LI
\fI-c cprogfile\fP:
This option tells \fIawkcc\fP to leave a C program in
\fIcprogfile\fP instead of the default \fIawker.c\fP.
.LI
\fI-b\fP:
"-b" specifies that \fIawkcc\fP write code that flushes its output buffers 
after each call to \fIprintf\fP or \fIprint\fP.  The result is expensive, 
but necessary
if the user intends to mix \fIgetline\fPs and \fIprint\fPs to the
same file without intervening calls to \fIclose\fP, or if a \fIsystem\fP
call makes use of a file the \fIawk\fP program has written to but not 
\fIclose\fPd.  \fIAwkcc\fP programs never buffer output to the terminal.
.LI
\fI-B\fP:
Specify that \fIawkcc\fP create programs which buffer their output;
this is currently the default.
.LE
.P
It is also possible to set these options in the script itself.
Comments beginning with the
word "awkcchint" are examined by \fIawkcc\fP, and if something
which could be interpreted as an argument follows, then
it has the expected result.  For instance, the user might specify
a script like the one which follows:
.DS
	$ cat awkscript.awk
	#awkcchint -c prog.c
	#awkcchint -o prog
	BEGIN { print "hello world" }
	$
.DE
Compiling this script with "awkcc awkscript.awk" is
similar to invoking "awkcc -c prog.c -o prog awkscript.awk".
.P
The #awkcchint facility is recommended for scripts that need to be compiled
with the "-b" option or "-u \fIvar\fP" specifications.
.H 2 "Internal organization"
.P
\fIAwkcc\fP is divided into two distinct components, the translator and
the run-time library.  The translator is responsible for examining
\fIawk\fP programs, producing appropriate C code, and calling \fIcc\fP to
compile the C code.  \fIcc\fP then performs the compilation and links
the resulting .o file with a run-time library.  Because
\fIawk\fP is such a high-level language some of its operations are fairly
complex;  to keep the size of the object file to a minimum not
all code is written in-line.  The run-time library is a repository
for all the functions \fIawkcc\fP does not want to replicate in the
\fIawker.c\fP file.
.H 3 "The translator."
The translator itself is composed of four elements:  a lexical
analyzer, a parser, an "optimizer", and a code generator.  The
job of the lexical analyzer (or "lexer") is to examine the
source file and break it into tokens understood by the parser.
The parser then constructs an abstract syntax tree (or AST)
which the optimizer then examines and changes whenever certain
conditions are met.  The modified syntax tree is then passed
to a code generator, which descends the tree recursively, writing
C code to the appropriate output file.
.P
This is a typical compiler organization, and in fact closely
parallels the organization of an interpreter: the two each
have a lexer and parser, but instead of routines to execute
the AST, a compiler has units which modify the tree and then
use it to direct the production of target-language code.
.P
The "optimization" step identifies opportunities for performance
savings.  Some of its tasks are:
.BL
.LI
to determine which variables are always used as numbers,
.LI
to determine when variables' numeric values or string values
are available.
.LI
to discover that it may not be necessary to parse every record fully, and
.LI
to find special-case situations in which simple routines can
be substituted for more complex ones (for instance, some regular
expression matching can be solved with a simple call to \fIstrcmp\fP).
.LE
.P
Certain optimizations like common subexpression elimination,
constant folding, and redundancy elimination might be particularly useful 
given the nature of the "typical" \fIawk\fP program.  These are
not implemented in the current version of \fIawkcc\fP, but are
candidates for future work.
.H 3 "The run-time library and header files."
.P
The run-time library contains a number of compiled source files
which may or may not be linked to the awker.o file.  It includes
procedures so complex that writing them into the C
program does not make sense.  The fact that these functions are
not necessary to every \fIawk\fP program is an added bonus and
helps reduce the size of the object file.
.P
There are also a number of header files which \fIcc\fP includes with
its "-I" mechanism.  In these header files are a number of macro
definitions which perform operations too inexpensive to warrant
the overhead of a function call.
.H 2 "When should one use \fIawkcc\fP?"
.H 3 "Its role in program development."
.P
\fIAwk\fP can be invoked conveniently with simple throw-away programs
written on the command line, or with a program stored in some
file.  Invoking \fIawkcc\fP and subsequently calling the new 
\fIa.out\fP adds overhead to the development cycle.  However, 
\fIawkcc\fP is not intended for use with programs still under 
development but rather with stable, "finished" products.
Thus the write-test cycle that \fIawk\fP programmers are accustomed
to is not unduly stressed by \fIawkcc\fP.
.H 3 "Which programs should be translated."
.P
The extent to which \fIawkcc\fP improves execution time of \fIawk\fP
code varies from \fIawk\fP program to \fIawk\fP program.  It is
possible, for instance, to conceive of a program which does
not benefit at all from compilation:  the program that does
nothing.  Here, the overhead involved in simply spawning a
new process dominates the execution time of the program regardless
of whether the program is compiled or interpreted, so compilation
is pointless.  A rule of thumb is:  unless running a program is
expensive, one should not compile it.
.P
The greatest improvements in
execution time will come from compiling programs that spend
the bulk of their time performing low-level operations like
arithmetic.  High-level programs cannot be improved as much
by compilation;  this is because high-level operations (for
instance, \fIsub\fP and \fIgsub\fP in \fIawk\fP) are probably going 
be library routines in either mode, and these functions
will probably be implemented with roughly equal efficiency.
A different way to think of this is to note that walking the program
graph (which interpreters must do) can often be more
expensive than performing the operations specified by the program
graph, but the converse is also true.
.P
Other programs that can be compiled with beneficial results
are very large programs.  If the program is interpreted at
every invocation, then it is also being parsed with every
invocation - a high price to pay if the program is very
large.  \fIAwkcc\fP eliminates this startup cost.
One large (809 line)
program in Allentown became 10 times faster after compilation for just
this reason.
.P
Improvements also come from the fact that compilers have
no qualms about spending time in up-front analysis of
a program.  This examination of programs tends to reveal
suitable places for performance improvements, although there
do not seem to be any convenient ways to spot which programs
will benefit most from these.
.H 3 "Sometimes C programs need not be written."
.P
One should also consider whether \fIawk\fP is a suitable language
for a particular problem - is it possible to produce code
with \fIawk\fP which rivals that one might produce in a lower-level 
language like C?
.P
The way an \fIawk\fP program uses implicit action (like	record-splitting)
and high-level constructs (like associative arrays)
can indicate the extent to which hand re-coding might be
advantageous.  For instance, consider a program which
counts the number of times the first field in a file is
repeated:
.DS
	{ keys[$1]++ }
	END { for (i in keys) print keys[i], i }
.DE
This program will probably not benefit greatly from hand-coding in
C because it uses associative arrays in a way that
has no cheap equivalent, and it requires record-splitting.  
This implies that the C programmer will
be forced to invent his own hashing and record-splitting
libraries at a great time loss, or import someone else's.  
In neither case is it likely that these libraries will be
significant improvements over the ones \fIawk\fP and \fIawkcc\fP 
already possess.  On one large file, the current version of \fIawkcc\fP 
executes this program 3.8 times more quickly than the current version 
of \fIawk\fP, and the compiled version is roughly equivalent to 
what a good C programmer would write from scratch.
.P
Alternately, consider a program which sorts its input:
.DS
	{ lines[NR]=$0 }
	END { 
		qsort(1,NR);
	 	for (i=1; i<=NR; i++) print lines[i];
	}
	func qsort(b,t) {
		# appropriate code here
	}
.DE
Here, the use of associative arrays,
signals that there is room for tremendous improvement by a C
programmer.  Associative arrays are being used
to simulate "ordinary" numerically subscripted arrays - the
full power of hash tables is being thrown at a problem that
requires far less.  Jon Bentley has results showing that
for quicksort \fIawk\fP code will run 15+ times more slowly
than a good C program\*(Rf. The current version of \fIawkcc\fP
.RS
J.L. Bentley:  casual communications.
.RF
can make this \fIawk\fP program about about two and a half times
faster.
.H 2 "Miscellaneous"
.H 3 Transparency.
.P
In creating \fIawkcc\fP, one concern has traditionally involved
the question "but is it the same as \fIawk\fP?"  That is, does
\fIawkcc\fP compile programs to do the same thing that they would
if run through the \fIawk\fP interpreter?  This question has its
root in two sources.
.P
One first needs to know that the same language is being
accepted by the lexer/parser front ends to the interpreter
and compiler.  This is a question of syntax, and is answered by
the fact that \fIawkcc\fP uses slightly modified version of \fIawk\fP's
lex and yacc front end.  As we saw earlier, there is no reason
for this component of the translator to differ greatly from the
interpreter.
.P
The other difficulty is a problem of semantics:  does a program
mean the same to \fIawkcc\fP as it does to \fIawk\fP?  Some programs
do things not implied by the language definition, and people take 
advantage of these quirks.  When such unusual behavior is noted,
\fIawkcc\fP is usually changed to preserve compatability.
.H 3 Portability.
.P
\fIAwkcc\fP
has been used primarily on Sun 3/60's and VAXen running BSD4.3,
but bringing it up on other machines has not been problem.
The translator itself is quite portable, and the C code it produces
only slightly less so.
.P
Experience has shown that the C compilers on many machines are
configured for humans and not program generators, that is, 
certain constants are poorly tuned for 
\fIawkcc\fP programs.  Thus it has occasionally been necessary to
modify the \fIcc\fP source to reflect the kind of
program that \fIawkcc\fP has been known to create*.
.FS "*"
Two \fIpcc\fP constants in particular have been adjusted
to avoid "out of tree space" and "out of temporary string space"
errors.  These are TREESZ and NTSTRBUF, and they have been set to
1000 and 200, respectively, with good results.
.FE
However,
assuming the C compiler source is available, this is not a problem.
.H 3 Availability.
.P
\fIAwkcc\fP can be obtained by contacting the author.
.H 1 "Conclusion"
.P
For certain applications, \fIawkcc\fP has been known to
significantly decrease execution time of \fIawk\fP programs.
This has the effect of making \fIawk\fP more widely useful,
since more problems can be solved with implementations that
are "sufficiently" fast - that is, the user does not feel
a need to recode the application in a lower-level language.
.HU Acknowledgements
.P
I would like to thank Eric Sumner Jr., Ann Martin, and Al Aho,
whose support and enthusiasm have been invaluable.
Hai-Yann Hwang and Jon Bentley have done very interesting work 
in comparing the performance of the compiler and interpreter 
against C programs, for which I am grateful\*(Rf.
.RS
H.Y. Hwang: "Performance analysis of AWK, awkcc, and C", in draft.
.RF
.P
And no acknowledgement would be complete without a special note of
thanks for the first people to make use of a program.  In 
particular, Howard Lipson, Kalyan Mondal, and Howard Moscovitz 
have played a critical part of the development process and their 
reports have been most helpful.
.SG
