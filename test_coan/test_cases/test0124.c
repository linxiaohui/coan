/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
/* Left(unk) && Right(false,elim) := Elim */
#if 0 && defined(FOO2)
DELETE ME
#else
KEEP ME
#endif
