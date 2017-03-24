/**ARGS: symbols --once-only --expand --includes --active "-DFOO=<stdlib.b>" -DTRUE=1 */
/**SYSCODE: = 0 */
#if TRUE
#include FOO
#else
#include <stdio.h>
#endif

