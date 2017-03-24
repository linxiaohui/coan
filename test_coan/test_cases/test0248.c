/**ARGS: pragmas  --active --locate -DFOO -UBAR */
/**SYSCODE: = 0 */
#ifdef FOO
#pragma This /* comment
comment */\
is a   pragma
#endif
#ifndef BAR
#pragma This is   a pragma 
#endif

