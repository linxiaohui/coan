/**ARGS: source */
/**SYSCODE: = 1 | 2 | 16 */

#define MACRO(x,y) (x) + (y)
#define FOO 1
#define BAR 1

#if MACRO((FOO + 1),(BAR))
KEEP ME
#endif
#if MACRO((FOO - 1),(BAR - 1))
DELETE_ME
#endif
#if MACRO(FOO,BAR) == (FOO + BAR)
KEEP_ME
#endif


