/**ARGS: source -UABYSSINIA -UPYGMALION -UABELONE */
/**SYSCODE: = 1 | 16 */

#if (defined UNIX && defined PYGMALION) || (defined LINUX && defined ABELONE) || (defined MACOSX && defined ABYSSINIA)
extern int i;
#endif
#if (defined UNIX && defined PYGMALION) || (defined MACOSX && defined ABYSSINIA)
extern int j;
#endif
