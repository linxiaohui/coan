/**ARGS: includes -s -A -DFOO */
/**SYSCODE: = 0 */
#ifdef FOO
#include <foo.h>
#include "bar.h"
#include <foo.h>
#else
#include <foo.h>
#include "bar.h"
#endif
 
