/**ARGS: includes -L --active --once-only -DFOO -UBAR */
/**SYSCODE: = 0 */
#ifdef FOO
#include <foo.h>
#include <foo.h>
#else
#include <foo.h>
#include <foo.h>
#endif
#ifndef BAR
#include "bar.h"
#include "bar.h"
#else
#include "bar.h"
#include "bar.h"
#endif
