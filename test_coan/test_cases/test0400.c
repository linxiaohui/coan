/**ARGS: source '-DFOO=3 && || 4' */
/**SYSCODE: = 4 */
/**NO-OUTPUT **/
#if FOO
KEEP ME
#else
DELETE ME
#endif
