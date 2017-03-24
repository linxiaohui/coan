/**ARGS: source -DX */
/**SYSCODE: = 16 | 1 */
#if 0,0 == 0
KEEP ME
#else
DELETE ME
#endif
#if 0,1 == 0
DELETE ME
#else
KEEP ME
#endif
#if (0,1) == 1
KEEP ME
#else
DELETE ME
#endif
#if (4,3,1) == (3,2,1)
KEEP ME
#else
DELETE ME
#endif

