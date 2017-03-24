/**ARGS: source "-DFOO(x)=x+1" "-DBAR(x,y)=x-y"  */
/**SYSCODE: = 1 | 16 */
#ifdef FOO
KEEP ME
#endif
#ifdef BAR
KEEP ME
#endif
#if FOO(0)
KEEP ME
#endif
#if BAR(1,1)
DELETE ME
#endif

