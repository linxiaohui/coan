/**ARGS: defs --once-only --inactive -DFOO */
/**SYSCODE: = 2 */
#ifdef FOO
#define /* comment */ B /* comment */ " Definition  1 " // Comment
#else
#define /* comment */ B /* comment */ " Definition 2 " // Comment
#endif
