/**ARGS: source -DA=1 -DB=0 "-DC=(1 && 0)" */
/**SYSCODE: = 1 | 16 */
#if (A || B)
KEEP ME
#endif
#if (A && B)
DELETE ME
#endif
#if C
DELETE ME
#endif
