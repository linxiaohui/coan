/**ARGS: symbols --locate -DFOO -UBAR*/
/**SYSCODE: = 0 */
#ifdef FOO
#if FOO1 == 1
#endif
#else
#if FOO2 == 1
#endif
#endif
#ifndef BAR
#if BAR1 == 1
#endif
#else
#if BAR2 == 1
#endif
#endif
