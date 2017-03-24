/**ARGS: symbols --locate --once-only --expand */
/**SYSCODE: = 2 */
#define FOO 1
#define FOO 1
#define BAR 2
#define BAR 2
#undef FOO
#define FOO 3
#undef BAR
#define BAR 4

