/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 32 */
#if !defined(FOO1) && !defined(UNKNOWN) || defined(UNKNOWN) && defined(FOO1)
KEEP ME
#else
KEEP ME
#endif
