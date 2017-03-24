/**ARGS: source -DFOO=X -UBAR*/
/**SYSCODE: = 32 | 1 */
#if (defined(FOO) ? defined(BAR) ? BAR : FOO : 2) == 3
KEEP ME
#else
DELETE ME
#endif
