/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#ifdef FOO1
KEEPME
#else
#undef FOO1
#endif
