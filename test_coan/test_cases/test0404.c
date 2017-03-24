/**ARGS: source -DFOO=1 -UBAR */
/**SYSCODE: = 16 | 1 */
#if defined(FOO) ? FOO : BAR
KEEP ME
#else
DELETE ME
#endif

