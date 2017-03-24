/**ARGS: source  */
/**SYSCODE: = 1 | 2 | 16 */

#define FOO 1
#define MACRO(x) x

#if FOO || BAR
#if MACRO((FOO) || (BAR))
KEEP ME
#endif
#endif

