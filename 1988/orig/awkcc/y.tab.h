/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 1988 AT&T
 *      All Rights Reserved

 * Note: Bell Laboratories was previously part of Lucent Technologies, later part
 * of Alcatel-Lucent, and now part of Nokia; some copyrights may have been
 * assigned by AT&T to its successors. This license is granted by Nokia solely to
 * the extent of its right to do so.

 */

typedef union  {
	Node	*p;
	Node	*cp;
	int	i;
	uchar	*s;
} YYSTYPE;
extern YYSTYPE yylval;
# define FIRSTTOKEN 257
# define PROGRAM 258
# define PASTAT 259
# define PASTAT2 260
# define XBEGIN 261
# define XEND 262
# define NL 263
# define ARRAY 264
# define MATCH 265
# define NOTMATCH 266
# define MATCHOP 267
# define FINAL 268
# define DOT 269
# define ALL 270
# define CCL 271
# define NCCL 272
# define CHAR 273
# define OR 274
# define STAR 275
# define QUEST 276
# define PLUS 277
# define AND 278
# define BOR 279
# define APPEND 280
# define EQ 281
# define GE 282
# define GT 283
# define LE 284
# define LT 285
# define NE 286
# define IN 287
# define ARG 288
# define BLTIN 289
# define BREAK 290
# define CLOSE 291
# define CONTINUE 292
# define DELETE 293
# define DO 294
# define EXIT 295
# define FOR 296
# define FUNC 297
# define SUB 298
# define GSUB 299
# define IF 300
# define INDEX 301
# define LSUBSTR 302
# define MATCHFCN 303
# define NEXT 304
# define ADD 305
# define MINUS 306
# define MULT 307
# define DIVIDE 308
# define MOD 309
# define ASSIGN 310
# define ASGNOP 311
# define ADDEQ 312
# define SUBEQ 313
# define MULTEQ 314
# define DIVEQ 315
# define MODEQ 316
# define POWEQ 317
# define PRINT 318
# define PRINTF 319
# define SPRINTF 320
# define ELSE 321
# define INTEST 322
# define CONDEXPR 323
# define POSTINCR 324
# define PREINCR 325
# define POSTDECR 326
# define PREDECR 327
# define VAR 328
# define IVAR 329
# define VARNF 330
# define CALL 331
# define NUMBER 332
# define STRING 333
# define FIELD 334
# define REGEXPR 335
# define CLINE 336
# define GETLINE 337
# define RETURN 338
# define SPLIT 339
# define SUBSTR 340
# define WHILE 341
# define CAT 342
# define NOT 343
# define UMINUS 344
# define POWER 345
# define DECR 346
# define INCR 347
# define INDIRECT 348
# define LASTTOKEN 349
