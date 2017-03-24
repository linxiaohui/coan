/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 2 | 16 | 32 */
#ifdef FOO1
#undef FOO1
KEEP ME
#endif
#ifdef FOO1
KEEP ME
#else
DELETE ME
#endif
