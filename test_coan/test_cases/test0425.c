/**ARGS: source -DZ */
/**SYSCODE: = 16 | 2 | 1 */
#define X 1
#define Y 2
#define FOO(X,Y) (X+Y)
#if FOO(3,4) == 7
KEEP ME
#else
DELETE ME
#endif
