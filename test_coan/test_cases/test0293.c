/**ARGS: source -DB */
/**SYSCODE: = 1 | 2 | 16 | 32 */

#ifdef B /* Something */
#undef B /* Something */
#define C /* Something */
#define B /* Something */
#endif

