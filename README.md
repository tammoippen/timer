# Timer Classes

This library provides three C++, stand-alone timer classes for simple benchmarking of your software.

## Stopwatch

The Stopwatch class is the simplest of the timers and all further timers use the Stopwatch for time measurement. It accumulates time between `start` and `stop`, and provides the elapsed time with different time units.  The Stopwatch class is partly inspired by [com.google.common.base.Stopwatch.java](https://code.google.com/p/guava-libraries/source/browse/guava/src/com/google/common/base/Stopwatch.java). The class is not thread-safe: do not share stopwatches among threads, but let each thread have its own stopwatch.

The basic time measuring is done via timestamps of `unsigned longs` representing mircoseconds:

```C++
#include <sys/time.h>
// ...
timer::Stopwatch::timestamp_t
timer::Stopwatch::get_timestamp()
{
  struct timeval now;
  gettimeofday( &now, ( struct timezone* ) 0 );
  return ( timer::Stopwatch::timestamp_t ) now.tv_usec
    + ( timer::Stopwatch::timestamp_t ) now.tv_sec *   timer::Stopwatch::SECONDS;
}
```

The usage is similar to a “real” stopwatch with a start, stop and reset button:

```C++
#include "stopwatch.hpp"

Stopwatch x;
x.start();
// ... do computations for 15.34 sec
x.stop(); // only pauses stopwatch
x.print( "Time needed " ); // > Time needed 15.34 sec.
x.start(); // resumes stopwatch
// ... next computations for 11.22 sec
x.stop();
x.print( "Time needed " ); // > Time needed 26,56 sec.
x.reset(); // reset to default values
x.start(); // starts the stopwatch from 0
// ... computation 5.7 sec
x.print( "Time " ); // > Time 5.7 sec.
// ^ intermediate timing without stopping the stopwatch
// ... more computations 1.7643 min
x.stop();
x.print("Time needed ", Stopwatch::MINUTES, std::cerr);
// > Time needed 1,8593 min. (on cerr)
// other units and output streams possible
```

## ScopeTimer

The `ScopeTimer` accumulates the elapsed times between creation and destruction of `ScopeTimer` objects with the same name. This can be very useful, if the overall execution time of a certain scope is to be measured, by creating a ScopeTimer at the start of a scope and relay on the destruction of the object at the end of the scope. The results are displayed automatically at the end of the program. This class is thread safe, as it measures the execution time separately for each thread (as long as threading is done via OpenMP). Thanks to Thorsten Hater for instpiration. A usage example:

```C++
#include "scopetimer.hpp"

{ // start of some scope
   ScopeTimer ot( "outside for-loop" );
   for ( size_t i = 0; i < 5; ++i )
   {
     ScopeTimer t( "in for loop" );
     // ... do computations for 5.83 sec each
   }
 } // end of some scope
 // ... other stuff
 // output at program exit:
 Collected Timers for thread  0
                in for-loop (calls    5) :: 29.156 sec.
           outside for-loop (calls    1) :: 29.1562 sec.
```

The `ScopeTimer` maintains some globale state for managing the different scopes. if this is not desired, you can disable the `ScopeTimer` by configuring with `-Denable-scopetimer=OFF`.

## SeriesTimer

A SeriesTimer object stores subsequent timings in a vector and is able to perform basic statistical analysis on the time series. It is not thread-safe: do not share SeriesTimer among threads, but let each thread have its own SeriesTimer.

It has methods to get all timings in a certain time unit, to sum all timings, get the average and standard deviation, and can compute [quantiles](https://en.wikipedia.org/wiki/Quantile). The usage is similar to the Stopwatch class, except that it has more methods for evaluating the time series:

```C++
#include "seriestimer.hpp"

SeriesTimer x;
for ( size_t i = 0; i < 10; ++i )
{
  x.start();
  // do computation for about 1 sec ...
  x.stop();
}
x.print("Timings: ");
//  Timings: (sec) [1.00134, 1.00134, 1.00134, 1.00137, 1.00134,
//                  1.00095, 1.00134, 1.00136, 1.00064, 1.00134 ]
//  Statistics:
//               sum = 10.0124
//              mean = 1.00124
//               std = 0.000231707
//        q 0% (min) = 1.00064
//             q 25% = 1.00134
//    q 50% (median) = 1.00134
//             q 75% = 1.00134
//      q 100% (max) = 1.00137
cout << "Sum: " << x.sum(Stopwatch::SECONDS) << " sec." << endl;
cout << "Avg: " << x.mean() << " sec." << endl; // default is sec
cout << "Std: " << x.std() << " sec." << endl;
x.reset(); // clear the timings
```

# Building

```sh
cmake -DCMAKE_INSTALL_PREFIX=<prefix> \
      -DCMAKE_BUILD_TYPE=Release \
      <src>
make
make install
```

Further options:

```
  -Denable-timing=[ON|OFF]     En/Disable timing altogether. [default=ON]
  -Denable-scopetimer=[ON|OFF] En/Disable ScopeTimer. [default=ON]
```

# Linking

To link the contained example, you could perform the following steps:

```
$(CXX) -fopenmp -L<prefix>/lib -ltimer -I<prefix>/include -Wl,-rpath,<prefix>/lib -o example src/example.cpp
```
