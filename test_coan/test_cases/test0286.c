/**ARGS: source -DFOO */
/**SYSCODE: = 1 | 2 | 16 */

#define A "def a"
#define B "def b"
#undef C
#ifdef A
KEEP ME
#endif
#ifndef A
DELETE ME
#endif
#ifdef C
DELETE ME
#endif

