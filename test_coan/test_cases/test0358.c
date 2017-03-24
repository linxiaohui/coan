/**ARGS: symbols -UFOO --no-transients --locate --expand --inactive */
/**SYSCODE: = 2 */

#define A 1
#undef B

#ifdef A
#define C
#endif

#ifndef B
#define D
#endif

#ifdef FOO
#define E
#endif

