/**ARGS: source "-DX=(65<<8)|66" */
/**SYSCODE: = 1 | 2 | 16 */
#if X == 'AB'
KEEP ME
#endif
#if (65 << 16) | (66 << 8) | 67 == 'ABC'
KEEP ME
#endif
#if (65 << 24) | (66 << 16) | (67 << 8) | 68 == 'ABCD'
KEEP ME
#endif
#if (65 << 32) | (66 << 24) | (67 << 16) | (68 << 8) | 69 == 'ABCDE'
KEEP ME PLUS ENDIF
#endif
#if 65L == L'A'
KEEP ME
#endif
#if X == L'AB'
KEEP ME
#endif
#if 0x04A8 == L'Ҩ'
KEEP ME
#endif


