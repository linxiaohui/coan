/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
/* Left(false,keep) || Right(true,elim) := Elim */
#if 0 || defined(FOO1)
KEEP ME
#else
DELETE ME
#endif
