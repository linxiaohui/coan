/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 2 | 16 | 32 */
#ifdef UNKNOWN
KEEP ME
#undef FOO1
#endif
#ifdef FOO1
KEEP ME
#else
DELETE ME
#endif
