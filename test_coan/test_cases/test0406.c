/**ARGS: source -DFOO=1 */
/**SYSCODE: = 32 | 1 */
#if !defined(FOO) ? FOO : BAR
KEEP ME
#else
DELETE ME
#endif

