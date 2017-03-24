/**ARGS: source -DFOO */
/**SYSCODE: = 1 | 16 */
#if 0x10u == 16U
KEEP ME
#endif
#if (020l == 16L)
KEEP ME
#endif
#if 020uL != 0x10Ul
DELETE ME
#endif
