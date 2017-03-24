/**ARGS: source -DFOO=1 */
/**SYSCODE: = 1 | 2 | 16 | 32 */

#define XXXX(ftbl, rval, func, ...) (ftbl,rval,func,...)

#if XXXX(ftbl,rval,func,arg)
KEEP_ME PLUS ENDIF
#endif

#if (XXXX(ftbl,rval,func,arg) + 2) 
KEEP_ME PLUS ENDIF
#endif

#if (XXXX(ftbl,rval,func,arg) || FOO) 
KEEP_ME
#endif

#if (FOO && XXXX(ftbl,rval,func,arg)) 
KEEP_ME PLUS ENDIF
#endif

