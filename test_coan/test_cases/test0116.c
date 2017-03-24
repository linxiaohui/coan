/**ARGS: source -E -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#if 1 || defined(UNKNOWN)
KEEP ME
#else
DELETE ME
#endif
