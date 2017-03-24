/**ARGS: source -DFOO=1  -DBAR */
/**SYSCODE: = 1 | 16 */
#if /* "comment" */ FOO /* 'comment'
more comments */ == 1
KEEP ME
#else /* coments
more comments
and "more
comments " */
DELETE ME
#endif
#if /* Comments "
more comments */ defined(BAR)
KEEP ME
#endif
