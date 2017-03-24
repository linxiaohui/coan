/**ARGS: symbols --explain --locate --select SYM -DFOO=1  -DBAR=FOO -DSYM=BAR -UVAL -DVAR=VAL */
/**SYSCODE: = 0 */
#if SYM  == 1
KEEP ME
#else
DELETE ME
#endif
#if defined(BAR)
KEEP ME
#endif
#ifndef VAR
DELETE ME
#endif
