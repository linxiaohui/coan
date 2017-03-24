/**ARGS: symbols --locate --once-only */
/**SYSCODE: = 0 */
#ifdef FOO1
KEEP ME
#endif
#if defined(FOO2) && defined(FOO2)
KEEP ME
#elif defined FOO3 || defined FOO4
KEEP ME
#elif FOO5
KEEP ME
#elif FOO1
KEEP ME
#elif FOO5
KEEP ME
#endif
