/**ARGS: source -UFOO */
/**SYSCODE: = 32 | 1 */

#if(defined(BAR)||defined(FOO))
KEEP ME
#endif
#if((defined(BAR))||(defined(FOO)))
KEEP ME
#endif

