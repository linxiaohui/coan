/**ARGS: includes --locate -DFOO */
/**SYSCODE: = 0 */
#ifdef FOO
#include <foo*&bar.h>
#elif 1
//KEEP ME
#endif



