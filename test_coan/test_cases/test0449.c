/**ARGS: source -DX */
/**SYSCODE: = 16 | 2 | 1 */
#define CAT2(x,yy) x ## yy
#define CAT3(x,yy,zzz) CAT2(x,CAT2(yy,zzz)) 
#if CAT3(11,222,3) == 112223
KEEP_ME
#else
DELETE ME
#endif
