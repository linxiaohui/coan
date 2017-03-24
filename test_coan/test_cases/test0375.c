/**ARGS: symbols -A -L --explain --select YY*,Z*,A -DXX=A */
/**SYSCODE: = 3 */

#define YY XX
#define ZZ YY
#define BB A
#define A 1
#if YY == BB
#undef A
#else
#undef BB
#endif
