/**ARGS: source -DFOO=1 -DBAR=1+1 */
/**SYSCODE: = 1 | 16 */
#if (1 + BAR) && defined(FOO)
KEEP ME
#endif
