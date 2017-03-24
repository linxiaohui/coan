/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 32 | 16 */
#ifdef UNKNOWN
KEEP ME
#elif defined(FOO1)
KEEP ME
#else
DELETE ME
#endif
