/**ARGS: symbols --explain --select S* -DFOO=1+1 -DBAR=3 -DSYM=FOO+BAR */
/**SYSCODE: = 0 */
#if SYM
KEEP ME
#endif
