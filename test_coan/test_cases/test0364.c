/**ARGS: symbols --explain -DFOO=1 --locate -DBAR=FOO -DSYM=BAR -UVAL -DVAR=VAL */
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
