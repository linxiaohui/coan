/**ARGS: source '-DFOO(x,y)=(x+y)' */
/**SYSCODE: = 16 | 1 */
#if FOO(/**/1/**/,/**/2/**/)/**/==/**/3
KEEP ME
#else
DELETE ME
#endif
