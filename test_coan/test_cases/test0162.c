/**ARGS: source -DFOO=3 -DBAR=0 */
/**SYSCODE: = 1 | 16 */
#if FOO & BAR
DELETE ME
#endif
#if FOO & 2
KEEP ME
#endif
#if FOO ^ BAR
KEEP ME
#endif
#if FOO ^ FOO
DELETE ME
#endif
#if FOO | BAR
KEEP ME
#endif
#if BAR | BAR
DELETE ME
#endif
#if ~BAR
KEEP ME
#endif
#if FOO & ~FOO
DELETE ME
#endif
