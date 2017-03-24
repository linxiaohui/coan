/**ARGS: source --pod -DFOO -DBAR */
/**SYSCODE: = 16 | 2 | 1 */
#ifdef FOO
/* Not to be parsed as comment
#if defined(BAR)
KEEP ME
#else
DELETE ME
#endif
end */
" Not to be parsed as quotation
#else
DELETE ME
#endif
end "
