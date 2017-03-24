/**ARGS: source -DFOO=3 -UBAR*/
/**SYSCODE: = 16 | 1 */
#if (defined(FOO) ? defined(BAR) ? BAR : FOO : 2) == 3
KEEP ME
#else
DELETE ME
#endif
