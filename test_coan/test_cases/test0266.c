/**ARGS: source -DX=1 -DY=32 */
/**SYSCODE: = 16 | 2 | 1 */
#if ( X << Y )
KEEP ME
#endif
#if ( X >> Y )
KEEP ME
#endif
#if 1L << 64
KEEP ME
#endif
#if 1 << -2
KEEP ME
#endif
#if 1 << -2U
KEEP ME
#endif

