/**ARGS: source -USYS2 */
/**SYSCODE: = 1 | 32 */

#if (defined(SYS1) && defined(ATTR1)) || (defined(ATTR1) || defined(SYS3)) || defined(SYS2) 
KEEP ME
#endif

