/**ARGS: source  -DX */
/**SYSCODE: = 6 */
/**NO-OUTPUT */
#define STR(x) # x
#define STRING(y) STR(y)
#if STRING(aa) 
KEEP ME
#else
KEEP ME
#endif
