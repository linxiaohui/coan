/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#if !defined(FOO1) || !defined(UNKNOWN) && defined(UNKNOWN) || defined(FOO1)
KEEP ME
#else
DELETE ME
#endif
