/**ARGS: source -DX */
/**SYSCODE: = 16 | 2 | 1 */
#define FOO(x,y) (x) + (y)
#if FOO(FOO(1,2),FOO(1,2)) == 6
KEEP ME
#else
DELETE_ME
#endif
