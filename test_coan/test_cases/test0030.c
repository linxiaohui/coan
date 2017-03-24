/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 32 */
#if defined(FOO1) && defined(UNKNOWN)
KEEP ME
#else
KEEP ME
#endif

