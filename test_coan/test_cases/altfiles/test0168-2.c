#define BAR (A,B)
#undef X
#ifdef FOO1
KEEP ME
#endif
#if defined(FOO2) && defined(FOO2)
KEEP ME
#elif defined FOO3 || defined FOO4
KEEP ME
#elif FOO6
KEEP ME
#endif
