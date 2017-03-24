/**ARGS: source -E -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#if 0 || defined(FOO2)
DELETE ME
#else
KEEP ME
#endif
