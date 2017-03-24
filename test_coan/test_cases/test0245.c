/**ARGS: defs --active --once-only -DFOO */
/**SYSCODE: = 2 */
#ifdef FOO
#define FOO /* comment */ 1
#define FOO 2
#define FOO 1
#undef BAR
#undef BAR //
#undef /*
*/ BAR
#else
#define SYM
#endif

