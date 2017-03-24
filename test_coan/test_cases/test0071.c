/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#ifndef FOO1
#undef FOO1
#else
KEEP ME
#endif
