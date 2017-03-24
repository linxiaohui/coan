/**ARGS: source -DFOO=1 */
/**SYSCODE: = 32 | 1 */
#if defined(FOO) ? BAR : FOO
KEEP ME
#else
DELETE ME
#endif

