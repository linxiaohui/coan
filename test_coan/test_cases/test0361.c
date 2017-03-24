/**ARGS: source -DFOO */
/**SYSCODE: = 16 | 2 | 1 */
/* Test case for sourceforge bug 3531311 */

#define TOKEN/*comment*/1//Comment
#if TOKEN == 1
KEEP ME
#else
DELETE ME
#endif
#define MAC(x)/*comment*/((x)+(x))/**///
#if MAC(1) == 2
KEEP ME
#else
DELETE ME
#endif

