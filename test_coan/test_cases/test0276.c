/**ARGS: source -DFOO */
/**SYSCODE: = 1 | 2 | 16 */
#define BAR(param) (param < 5)
#if BAR(4)
KEEP ME
#endif
