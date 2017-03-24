/**ARGS: symbols --explain */
/**SYSCODE: = 2 */
#define type_comb2(T1, T2) __typeof__(0 ? (T1)0 : (T2)0)
#define type_comb3(T1, T2, T3) type_comb2(T1, type_comb2(T2, T3))
#define type_comb4(T1, T2, T3, T4)				\
	type_comb2(T1, type_comb2(T2, type_comb2(T3, T4)))
