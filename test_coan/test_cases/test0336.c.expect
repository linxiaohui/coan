/**ARGS: source */
/**SYSCODE: = 2 */
/* Problem code from Tony Whitely */


#ifndef __RTXCLOCK_H__
#define __RTXCLOCK_H__

#include <cstdio>

#define ITERCNT 100000L

#ifndef MSC_VER      /* not MSVC++; does this work for Borland? */

#ifdef T_XXXXX /*  */
#include <common/time.h>
#else
#include <time.h>
#endif /*T_XXXXX*/

#define DECLARE_BENCHMARK \
   timespec tp0, tp1; \
   long __clk__ = 0;

#define CLOCK_START \
   clock_gettime(CLOCK_REALTIME, &tp0);

#define CLOCK_STOP \
   clock_gettime(CLOCK_REALTIME, &tp1);

#define BEGIN_LOOP \
   CLOCK_START \
   for (__clk__ = 0; __clk__ < ITERCNT; __clk__ ++) {

#define END_LOOP \
   } \
   CLOCK_STOP

#define PRINT_RESULTS_MS \
   long _ds = tp1.tv_sec - tp0.tv_sec, \
        _dn = tp1.tv_nsec - tp0.tv_nsec; \
   double _dms = (_ds * 1000.f) + (_dn / 1e6); \
   printf ("\t%.6f\t%.6f\n", _dms, _dms/(float)ITERCNT);

#else                /* MSVC++ */

#include <windows.h>

#define DECLARE_BENCHMARK \
   LARGE_INTEGER _start, _stop, _freq; \
   long __clk__ = 0; \
   QueryPerformanceFrequency(&_freq);

#define CLOCK_START \
   QueryPerformanceCounter(&_start);

#define CLOCK_STOP \
   QueryPerformanceCounter(&_stop);

#define BEGIN_LOOP \
   CLOCK_START \
   for (__clk__ = 0; __clk__ < ITERCNT; __clk__ ++) {

#define END_LOOP \
   } \
   CLOCK_END

#define PRINT_RESULTS_MS \
   double _delta = (_stop.QuadPart - _start.QuadPart), \
          _total = _delta / (double)_freq.QuadPart * 1000; \
   printf("\t%.6f\t%.6f\n", _total, _total/(float)ITERCNT);

#endif

#endif /* DEFINE __RTXCLOCK_H__ */

