/**ARGS: symbols -L -o -DFOO=0 -UBAR */
/**SYSCODE: = 0 */
#ifdef FOO
#if FOO == 1
#endif
#if FOO == 2
#endif
#else
#if FOO == 1
#endif
#if FOO == 2
#endif
#endif
#ifndef BAR
#if BAR == 1
#endif
#if BAR == 2
#endif
#else
#if BAR == 1
#endif
#if BAR == 2
#endif
#endif
