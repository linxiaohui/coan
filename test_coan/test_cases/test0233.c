/**ARGS: errors  -DFOO --active --locate */
/**SYSCODE: = 2 | 128 */
#ifdef FOO
#error An  /* comment */\
 error  /* a \
comment */   message
#else
#error An/* Comment */error/**/message
#endif

