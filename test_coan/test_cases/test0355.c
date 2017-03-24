/**ARGS: source --no-transients -DB */
/**SYSCODE: = 32 | 16 | 2 | 1 */

#define A 1
#undef B

#ifdef A
KEEP THIS AND IFDEF
#endif

#ifndef B
KEEP THIS AND IFNDEF
#endif

#if (A && 1)
KEEP THIS AND IF
#endif

#if (!A && 1)
KEEP THIS AND IF
#endif

