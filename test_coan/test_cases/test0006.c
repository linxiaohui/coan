/**ARGS: source -DFOO1 -UFOO2 */
/**SYSCODE: = 1 | 16 */
#ifdef FOO1
#include "header.h"
KEEP ME
#else
DELETE ME
#include "header1.h"
#endif

