/**ARGS: source  -DFOO=1 */
/**SYSCODE: = 1 | 16 */
#if FOO  /* "A quoted
string witin quotation" */ == 1 
KEEP ME
#else
DELETE ME
#endif
