/**ARGS: source  -DX */
/**SYSCODE: = 16 | 2 | 1 */
#define FOO(xx,yyyy) xx##yyyy 
#if FOO(00,1) == 1
KEEP ME
#else
DELETE_ME
#endif
