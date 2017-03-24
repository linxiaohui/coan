/**ARGS: source */
/**SYSCODE: = 2 */
#define A (1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1)
#define B (A + A + A + A + A + A + A + A + A + A + A + A + A + A + A + A)
#define C (B + B + B + B + B + B + B + B + B + B + B + B + B + B + B + B)
#define D (C + C + C + C + C + C + C + C + C + C + C + C + C + C + C + C)
#define E (D + D + D + D + D + D + D + D + D + D + D + D + D + D + D + D)
#define F (E + E + E + E + E + E + E + E + E + E + E + E + E + E + E + E)
#define G (F + F + F + F + F + F + F + F + F + F + F + F + F + F + F + F)
#define H (G + G + G + G + G + G + G + G + G + G + G + G + G + G + G + G)
#if H
KEEP ME
#else
KEEP ME
#endif
#if !H
KEEP ME
#else
KEEP ME
#endif
