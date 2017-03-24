/**ARGS: source -DFOO  -DBAR  */
/**SYSCODE: = 1 | 2 | 16 */
#ifdef FOO
#define A " /* comment within quotation \
is not comment. And escaped \" does not close quotation */ "
#endif
#if defined(BAR)
#define B '\
"\
\
' /* B is defined as '"'. Quotation is not opened */
#endif
