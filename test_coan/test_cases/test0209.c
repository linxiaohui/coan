/**ARGS: includes --locate --active -DFOO -UBAR */
/**SYSCODE: = 0 */
#ifdef FOO
#include <foo.h>
#else
#include "foo.h"
#endif
#ifndef BAR
#include "bar.h"
#else
#include <bar.h>
#endif



