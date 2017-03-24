#if Y(1,2)
KEEP ME
#endif
#if Y(3,4)
KEEP ME
#endif
#define BAR(A,B) (A + B)
#undef X
#if BAR(1,2)
KEEP_ME
#endif
#if BAR(0,1)
#if UNK(2)
KEEP ME
#endif
#if UNK(3)
KEEP ME
#endif
KEEP ME
#endif
#ifdef FOO1
KEEP ME
#endif
#if defined(FOO2) && defined(FOO2)
KEEP ME
#elif defined FOO3 || defined FOO4
KEEP ME
#elif FOO5
KEEP ME
#endif
