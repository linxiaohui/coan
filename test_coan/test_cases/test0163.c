/**ARGS: source "-DFOO=4>>2" */
/**SYSCODE: = 1 | 16 */
#if 1 << 2 == 4
KEEP ME
#else
DELETE ME
#endif
#if FOO == 1
KEEP ME
#else
DELETE ME
#endif


