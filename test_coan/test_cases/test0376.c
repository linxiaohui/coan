/**ARGS: symbols -L --active --explain --select C*,F* */
/**SYSCODE: = 2 */

#define AB 2
#define BC (AB + AB)
#define CD (BC * AB)
#define DE 8
#if CD == DE
#define EF DE
#else
#define FG
#endif

