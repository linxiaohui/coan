/**ARGS: symbols -L --expand -DFOO=1 */
/**SYSCODE: = 2 */

#define BAR (FOO ? 1 : 0)
#if BAR
#undef BAR
#endif

