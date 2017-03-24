/**ARGS: source -DNUM=1  */
/**SYSCODE: = 1 | 2 | 16 */
#define F\
OO\
(x\
x\
,yy\
)\
 xx\
*\
yy

#define\
 BA\
R(\
x,\
y)\
 x-\
y
 
#if FO\
O\
(\
B\
AR(\
NUM\
,\
NUM\
)\
,2\
) == -1
KEEP ME
#else
DELETE ME
#endif
#if FOO(1\
+\
1,BAR(NU\
M+NUM,0)) ==3
KEEP ME
#else
DELETE ME
#endif

