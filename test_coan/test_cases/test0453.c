/**ARGS: symbols --explain --locate */
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
#define I 1 
#else
#define J 0 
#endif
#if !H
#define K 1 
#else
#define L 0 
#endif
