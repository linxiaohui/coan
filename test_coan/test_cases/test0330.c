/**ARGS: source -DFOO=1 -DBAR=0  */
/**SYSCODE: = 4 */
#if (FOO || &%nonsense)
KEEP ME
#endif
#if (BAR && &%nonsense)
DELETE ME
#endif

