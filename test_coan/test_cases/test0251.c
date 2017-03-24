/**ARGS: symbols --expand "-DFOO=(1+1)" -DBAR=3 -DAND=And "-DSYM=(FOO AND (FOO And BAR) And BAR)" */
/**SYSCODE: = 4 */
#if SYM
KEEP ME
#endif
