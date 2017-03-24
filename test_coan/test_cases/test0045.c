/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 2 | 16 | 32 */
#undef FOO1
#ifdef FOO1
KEEP ME
#else
DELETE ME
#endif

