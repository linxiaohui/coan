/**ARGS: source -DFOO=1 -DBAR=1*/
/**SYSCODE: = 16 | 1 */
#if defined(FOO) ? (defined(BAR) ? FOO : BAR) : 0
KEEP ME
#else
DELETE ME
#endif

