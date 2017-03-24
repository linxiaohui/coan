/**ARGS: includes --locate --once-only */
/**SYSCODE: = 2 */
#define HDR <foo.h>
#include HDR
#undef HDR
#define HDR "bar.h"
#include HDR

