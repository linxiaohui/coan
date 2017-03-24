/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 32 */
#ifdef UNKNOWN
KEEP ME
#elif defined FOO1
KEEP ME
#endif

