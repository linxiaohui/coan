/**ARGS: source -DFOO */
/**SYSCODE: = 1 | 2 | 16 */

#define B 1
#ifdef A
KEEP ME
#endif
#ifdef B
KEEP ME
#endif
#ifdef C
KEEP ME
#endif

