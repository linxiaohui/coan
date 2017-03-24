/**ARGS: symbols --explain */
/**SYSCODE: = 3 */
#define FOO(x,y) BAR(x,y)
#define BAR(x,y) FOO(x+1,y+1)
#if FOO(0,0)
KEEP ME
#else
KEEP ME
#endif
