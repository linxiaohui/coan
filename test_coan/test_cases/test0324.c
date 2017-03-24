/**ARGS: source -DNUM=1 "-DBAR(x,y)=x-y" "-DFOO(x,y)=x*y"  */
/**SYSCODE: = 1 | 2 | 16 */
#if FOO(BAR(NUM,NUM),2)
KEEP ME
#else
DELETE ME
#endif
#if FOO(1+1,BAR(NUM+NUM,0))
KEEP ME
#else
DELETE ME
#endif
#define FOOBAR( x , y , z )  (FOO( x , y) || BAR(y ,z))
#if FOOBAR(0,1,0)
KEEP ME
#else
DELETE ME
#endif  
#if FOOBAR(0,NUM,NUM)
DELETE ME
#else
KEEP ME
#endif  
