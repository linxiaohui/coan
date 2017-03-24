/**ARGS: source  -DX */
/**SYSCODE: = 6 */
/**NO-OUTPUT */
#define FOO(x) x #y
#if FOO("ab\ab")
KEEP ME
#else
KEEP ME
#endif
