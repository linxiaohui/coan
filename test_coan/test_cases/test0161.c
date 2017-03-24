/**ARGS: source -DFOO=1 -DBAR=2 */
/**SYSCODE: = 1 | 16 */
#if 1 == (FOO | BAR == 3)
KEEP ME
#else
DELETE ME
#endif
