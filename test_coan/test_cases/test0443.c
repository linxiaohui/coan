/**ARGS: source  -DX */
/**SYSCODE: = 6 */
/**NO-OUTPUT */
#define FOO(x,y,z) '#' x "#" y # z 
#if FOO(aa,bbbb,cccc)
KEEP ME
#else
KEEP ME
#endif
