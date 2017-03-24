/**ARGS: source -DFOO=1 -DBAR=2 */
/**SYSCODE: = 1 | 16 */
#if 1 + BAR
KEEP ME
#endif
#if 1 - FOO
KEEP ME
#endif
#if 1 * FOO
KEEP ME
#endif
#if 1 / FOO
KEEP ME
#endif
#if 3 % BAR
KEEP ME
#endif
