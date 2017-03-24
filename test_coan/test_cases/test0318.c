/**ARGS: includes --locate --once-only --system */
/**SYSCODE: = 2 */
#define HDR "bar.h"
#include HDR
#undef HDR
#define HDR <foo.h>
#include HDR

