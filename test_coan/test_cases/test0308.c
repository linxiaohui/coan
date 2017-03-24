/**ARGS: source -UUNDEF  */
/**SYSCODE: = 1 | 2 | 16 */

#ifdef UNKNOWN
#define FOO
#endif

#ifndef UNDEF
#define BAR
#endif

#ifdef FOO
KEEP ME
#endif

#ifdef BAR
KEEP ME
#endif

