/**ARGS: source -m -DFOO=1 -UBAR */
/**SYSCODE: = 1 | 2| 16 | 32 */
#if BAR
DELETE ME
#else
KEEP ME
#endif
#define BAR
#if BAR
DELETE ME
#else
KEEP ME
#endif

