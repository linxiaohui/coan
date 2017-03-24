/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#ifndef FOO1
#define FOO1
DELETE ME
#endif
#ifdef FOO1
KEEP ME
#else
DELETE ME
#endif
