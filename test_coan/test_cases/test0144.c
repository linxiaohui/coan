/**ARGS: source -E -DFOO1=BAR */
/**SYSCODE: = 1 | 32 */
#if !(1 && FOO1)
KEEP ME
#else
KEEP ME
#endif
