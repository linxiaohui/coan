/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
/* Left(unk) || Right(true,elim) := Left */
#if 0 || defined(FOO1)
KEEP ME
#else
DELETE ME
#endif
