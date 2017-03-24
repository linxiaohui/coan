/**ARGS: symbols --explain --select X* "-DFOO=(1+1)" -DBAR=3 -DAND=And "-DSYM=(FOO AND (FOO And BAR) And BAR)" */
/**SYSCODE: = 4 */
/**NO-OUTPUT*/
#if SYM
KEEP ME
#endif
