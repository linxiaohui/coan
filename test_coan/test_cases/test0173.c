/**ARGS: source -DFOO=1 */
/**SYSCODE: = 1 | 16 */
#if FOO
KEEP ME
#else
DELETE ME
#endif
