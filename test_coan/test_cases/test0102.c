/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 32 */
#if 1 && defined(UNKNOWN)
KEEP ME
#else
KEEP ME
#endif
