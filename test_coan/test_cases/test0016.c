/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#ifdef UNKNOWN
KEEP ME
#elif !defined(FOO1)
DELETE ME
#endif
