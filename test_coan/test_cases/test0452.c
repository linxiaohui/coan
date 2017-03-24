/**ARGS: symbols --explain */
/**SYSCODE: = 6 */
/**NO-OUTPUT */
#define FOO(x,y) x##y
#if FOO('a','b')
KEEP ME
#else
KEEP ME
#endif
