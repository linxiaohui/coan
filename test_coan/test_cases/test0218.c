/**ARGS: symbols -L -I -DFOO=0 -UBAR */
/**SYSCODE: = 0 */
#ifdef FOO
#if FOO == 1
#endif
#else
#if FOO == 1
#define SYM 2
#endif
#endif
#ifndef BAR
#if BAR == 1
#endif
#else
#if BAR == 1
#undef SYM
#endif
#endif
 
