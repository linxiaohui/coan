/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 32 */
#if defined(UNKNOWN) || !defined(FOO1)
KEEP ME
#else
KEEP ME
#endif

