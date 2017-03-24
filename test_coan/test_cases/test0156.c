/**ARGS: source -DFOO=1 */
/**SYSCODE: = 1 | 32 */
#if (1 + BAR) && defined(FOO)
KEEP ME
#endif
