/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#if !defined(FOO1) && (defined(UNKNOWN) || defined(UNKNOWN))
DELETE ME
#else
KEEP ME
#endif
