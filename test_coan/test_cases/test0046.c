/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#undef FOO2
#ifdef FOO2
DELETE ME
#else
KEEP ME
#endif

