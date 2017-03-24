/**ARGS: source  -DFOO=1 */
/**SYSCODE: = 32 | 1 */
#if !(FOO && BAR)
KEEP_ME
#endif
#if -(FOO + BAR)
KEEP_ME
#endif
#if ~(FOO | BAR)
KEEP_ME
#endif

