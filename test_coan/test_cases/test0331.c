/**ARGS: source -DFOO  */
/**SYSCODE: = 1 | 2 | 16 */

#define LOCAL_MACRO
#undef UNDEF_LOCAL_MACRO
#if (defined (LOCAL_MACRO) && !defined (UNDEF_LOCAL_MACRO))
does_coan_keep_this_1();
#endif

