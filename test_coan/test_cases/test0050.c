/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 2 | 16 | 32 */
#ifndef FOO2
#define FOO2
KEEP ME
#endif
#ifdef FOO2
DELETE ME
#else
KEEP ME
#endif

