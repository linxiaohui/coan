/**ARGS: source --line -DFOO */
/**SYSCODE: = 1 | 16 | 32 */
#ifdef FOO
KEEP ME
#else
DELETE ME
DELETE ME
DELETE ME
#endif
#ifdef FOO
KEEP ME
#endif
