/**ARGS: includes --locate -DFOO "-DHDR(name)=<name.h>" */
/**SYSCODE: = 0 */
#ifdef FOO
#include HDR(limits)
#elif 1
//KEEP ME
#endif

