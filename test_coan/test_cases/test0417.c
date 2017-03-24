/**ARGS: source --implicit */
/**SYSCODE: = 4 */
/**NO-OUTPUT */
#if FOO(1)
KEEP ME 1
#else
KEEP ME 2
#endif
#if FOO
KEEP ME 3
#else
KEEP ME 4
#endif
