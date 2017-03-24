/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#define FOO1
#ifdef FOO1
KEEP ME
#else
DELETE ME
#endif

