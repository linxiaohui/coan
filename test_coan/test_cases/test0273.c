/**ARGS: source -DFOO=65 */
/**SYSCODE: = 1 | 16 */
#if FOO == 'A'
KEEP ME 1
#endif
#if FOO == 'A\
'
KEEP ME 2
#endif
#if FOO == '\
A'
KEEP ME 3
#endif
#if FOO == '\A'
KEEP ME 4
#endif
#if FOO == L'A'
KEEP ME 5
#endif
#if FOO == L\
'A'
KEEP ME 6
#endif
#if FOO == L'\A'
KEEP ME 7
#endif
#if FOO == '\0101'
KEEP ME 8
#endif
#if FOO == L'\0101'
KEEP ME 9
#endif
#if FOO == '\x41'
KEEP ME 10
#endif
#if FOO == '\\
x41'
KEEP ME 11
#endif
#if FOO == '\x\
41'
KEEP ME 12
#endif
#if FOO == '\x4\
1'
KEEP ME 13
#endif
#if \
FOO == 'A'
KEEP ME 14
#endif
#if FO\
O == 'A'
KEEP ME 15
#endif
#if FOO =\
= 'A'
KEEP ME 16
#endif
#\
if FOO == 'A'
KEEP ME 17
#endif
#i\
f FOO == 'A'
KEEP ME 18
#endif
#if defi\
ned FOO
KEEP ME 19
#en\
dif
#if 0x04A8 == L'\
Ҩ'
/* That isn't a G, it's a Ҩ */
KEEP ME 20
#endif

