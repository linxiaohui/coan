/**ARGS: symbols --explain */
/**SYSCODE: = 2 */
#define type_if_not(T, E) __typeof__(0 ? (T *)0 : (void *)(E))
#define type_if(T, E) type_if_not(T, !(E))
#define type_comb2(T1, T2) __typeof__(0 ? (T1)0 : (T2)0)
#define first_of2p(T1, E1, T2, E2) type_comb2(type_if(T1, (E1)),	   \
					     type_if(T2, (!(E1) && (E2))))
					     
first_of2p(T1,E1,T2,E2)
