/**ARGS: source -DFOO1 -UFOO2 -DBAR1 -UBAR2 */
/**SYSCODE: = 1 | 16 */
#ifdef FOO1
	#ifndef FOO2
	KEEP ME
	#else
	DELETE ME
	#endif
#else
	#ifdef BAR1
	DELETE ME
	#else
	DELETE ME
	#endif
#endif 
