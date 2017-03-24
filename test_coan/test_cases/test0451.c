/**ARGS: symbols  --explain --locate */
/**SYSCODE: = 6 */
/**NO-OUTPUT */
#define hash_hash # ## #
#define mkstr(a) # a
#define in_between(a) mkstr(a)
#define join(c, d) in_between(c hash_hash d)
#if join(x,y)
KEEP ME
#else
DELETE ME
#endif
