/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#ifdef FOO2
#undef FOO2
DELETE ME
#endif
#ifdef FOO2
DELETE ME
#else
KEEP ME
#endif

