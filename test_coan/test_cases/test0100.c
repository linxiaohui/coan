/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
/* Left(true,keep) && Right(false,elim) := Elim */
#if 1 && defined(FOO2)
DELETE ME
#else
KEEP ME
#endif
