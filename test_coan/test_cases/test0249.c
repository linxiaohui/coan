/**ARGS: directives  --active --once-only --locate -DFOO -UBAR "-DHDR=<time.h>" */
/**SYSCODE: = 2 | 128 */
#ifdef FOO
	#include HDR
	#ifndef BAR
		#pragma This /* comment
		comment */\
		is a   pragma
		#error Whoops /* An */ error //
		#define SYM 12345
		#define STR "1\\
2345"
	#endif
	#define FOO 12345
	#undef /*
	*/ HDR
#endif
#ifndef BAR
	#pragma This is   a pragma 
	#if defined(FOO)
		#include <time.h>
		#undef HDR
	#endif
	#define FOO 123\
45		/* comment
		*/
	#error Whoops  error
	#define SYM 123\\
45
	#define STR "1\2345"
#endif
#line 99

