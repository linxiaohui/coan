/**ARGS: source -DX=1U -DY=-1 */
/**SYSCODE: = 1 | 2 | 16 */
#if (X + Y) == 0
KEEP ME
#endif
#if X + Y
KEEP ME
#endif

