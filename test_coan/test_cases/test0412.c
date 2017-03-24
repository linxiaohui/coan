/**ARGS: source "-DFOO(T1,T2)=(T1+T2)" "-DBAR(T1,T2,T3)=FOO(T1,FOO(T2,T3))" */
/**SYSCODE: = 16 | 1 */
#if BAR(1,2,3) == 6
KEEP_ME
#else
DELETE ME
#endif



