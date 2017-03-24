/**ARGS: source -DFOO -UBAR */
/**SYSCODE: = 16 | 1 */
#if not defined(BAR)
KEEP ME
#else
DELETE ME
#endif
#if defined(FOO) and not defined(BAR)
KEEP ME
#else
DELETE ME
#endif
#if not defined(FOO) or not defined(BAR)
KEEP ME
#else
DELETE ME
#endif
#if compl 0
KEEP ME
#else
DELETE ME
#endif
#if 1 not_eq 2
KEEP ME
#else
DELETE ME
#endif
#if 1 xor 0
KEEP ME
#else
DELETE ME
#endif
#if 1 bit_and 1
KEEP ME
#else
DELETE ME
#endif
#if 1 bit_or 0
KEEP ME
#else
DELETE ME
#endif
