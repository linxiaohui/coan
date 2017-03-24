/**ARGS: source -E -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
/* Left(true,elim) || Right(true,elim) := Elim */ 
#if defined(FOO1) || 1
KEEP ME
#else
DELETE ME
#endif
