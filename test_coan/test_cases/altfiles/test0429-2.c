#if Y(1,2)
KEEP ME
#endif
#if Y(4,5)
KEEP ME
#endif
#define BAR(A,B) (A+B)
#undef X
#if BAR(1,2)
KEEP_ME
#endif
#if BAR(3,4)
#if UNK(2)
KEEP_ME
#endif
#if UNK(4)
KEEP_ME
#endif
KEEP_ME
#endif
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
