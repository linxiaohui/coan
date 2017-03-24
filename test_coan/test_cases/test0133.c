/**ARGS: source -E -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#if 0 || defined(FOO1)
KEEP ME
#else
DELETE ME
#endif
