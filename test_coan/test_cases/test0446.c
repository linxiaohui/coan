/**ARGS: source  -DX */
/**SYSCODE: = 6 */
/**NO-OUTPUT */
#define FOO(xx) # xx
#define BAR(yyy) #yyy
#if FOO(BAR(aaa)) 
KEEP ME
#else
KEEP ME
#endif
