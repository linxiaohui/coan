/**ARGS: includes --locate -DFOO*/
/**SYSCODE: = 2 */
#ifdef FOO
#include bar(x,y,z)
#elif 1
//KEEP ME
#endif

