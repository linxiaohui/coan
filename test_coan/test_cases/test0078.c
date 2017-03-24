/**ARGS: source -DFOO1 -UFOO2 -DBAR1 -UBAR2 */
/**SYSCODE: = 1 | 16 | 32 */
#if defined UNKNOWN
	#ifndef FOO2
	KEEP ME
	#else
	DELETE ME
	#endif
#elif defined FOO1
	#ifndef BAR2
	KEEP ME
	#else
	DELETE ME
	#endif
#elif !defined(BAR1)
	#if defined BAR1
	DELETE ME
	#elif !defined(BAR2)
	DELETE ME
	#endif
#endif
