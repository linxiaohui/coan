/**ARGS: symbols  --expand --active "-DFOO=1 + 1" */
/**SYSCODE: = 2 */
#ifdef FOO
#define FOO 1 +/* A Commment */1
#define FOO 2
#else
#define FOO 3
#endif

