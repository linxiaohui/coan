/**ARGS: source --eval-wip -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#if 1 && defined(FOO2)
DELETE ME
#else
KEEP ME
#endif
