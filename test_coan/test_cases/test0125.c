/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
/* Left(false,elim) && Right(unk) := Elim */
#if defined(FOO2) && 0
DELETE ME
#else
KEEP ME
#endif
