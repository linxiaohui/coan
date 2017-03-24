/**ARGS: source -DFOO=1 -DBAR=0 */
/**SYSCODE: = 1 | 16 */

#if (FOO ? 1 : 0)
KEEP_ME
#endif

#if (FOO ? (BAR ? 1 : 0) : 1)
DELETE_ME
#endif

#if (!defined (FOO) || FOO < 1 || \
     BAR < (defined (FOO) ? 1 : 0))
KEEP_ME
#endif

#if (!defined (FOO) || FOO < 1 || \
     BAR < (defined (FOO) ? 0 : 1))
DELETE_ME
#endif
