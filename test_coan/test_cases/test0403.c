/**ARGS: source -DFOO -UBAR */
/**SYSCODE: = 4 */
/**NO-OUTPUT */
#if defined(FOO) ? FOO : BAR
KEEP ME
#else
DELETE ME
#endif

