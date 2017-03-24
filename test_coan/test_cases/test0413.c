/**ARGS: symbols --expand --locate */
/**SYSCODE: = 3 */
#define FOO(x,y) (1 + BAR(x,y))
#define BAR(x,y) (1 + FOO(x,y))
#if BAR(1,2)
KEEP_ME
#else
DELETE ME
#endif
#if FOO(1,2)
KEEP_ME
#else
DELETE ME
#endif
