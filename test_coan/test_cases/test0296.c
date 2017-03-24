/**ARGS: source -kc -UDEFINE1 -UDEFINE2 -UDEFINE3 */
/**SYSCODE: = 1 | 32 */

uint32 var; 
 
#if defined(DEFINE1)              || \
     defined(DEFINE2)              || \
     defined(DEFINE3)
uint32 var = 0;
#endif

void main()
{
    return var;
}
