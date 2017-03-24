/**ARGS: source -DFOO=1 -DBAR=0 */
/**SYSCODE: = 4 */

#if (FOO ? 1)
KEEP_ME
#endif

