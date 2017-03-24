/**ARGS: symbols --no-transients --locate --expand */
/**SYSCODE: = 2 */

#define A 1
#undef B

#ifdef A
#define C
#endif

#ifndef B
#define D
#endif

