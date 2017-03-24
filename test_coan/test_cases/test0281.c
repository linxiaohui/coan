/**ARGS: source -m -DFOO=1  */
/**SYSCODE: = 1 | 16 */
#if BAR
DELETE ME
#else
KEEP ME
#endif
#if defined(BAR)
DELETE ME
#else
KEEP ME
#endif
#if !(BAR)
KEEP ME
#else
DELETE ME
#endif
#if BAR && FOO
DELETE ME
#else
KEEP ME
#endif
#if BAR < FOO
KEEP ME
#else
DELETE ME
#endif

