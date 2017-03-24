/**ARGS: source  -DX */
/**SYSCODE: = 6 */
/**NO-OUTPUT */
#define FOO(x,y)  #x # y 
#if FOO(aa,bbbb)
KEEP ME
#else
KEEP ME
#endif
