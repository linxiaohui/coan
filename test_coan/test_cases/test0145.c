/**ARGS: source -E -DFOO1=1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#if !(1 && FOO1)
DELETE ME
#else
KEEP ME
#endif
