/**
 * example.cpp
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Tammo Ippen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <iostream>

#include "scopetimer.hpp"
#include "seriestimer.hpp"
#include "stopwatch.hpp"
#include <unistd.h>

using namespace std;
using namespace timer;

unsigned long
fib( size_t n )
{
  // ScopeTimer f("inFib");
  if ( n == 0 )
    return 0;
  if ( n == 1 )
    return 1;
  return fib( n - 1 ) + fib( n - 2 );
}

int
main( int32_t argc, char const* argv[] )
{
  SeriesTimer y;
#pragma omp parallel num_threads( 4 )
  {
    ScopeTimer f( "fib" );
    Stopwatch x;


    y.start();
    x.start();
    fib( 43 );
    x.stop();
    y.stop();

    x.print( "First fib time = " );

    y.start();
    x.start(); // resume
    fib( 43 );
    x.print( "Some intermediate: " );
    x.stop();
    y.stop();

    x.print( "First & Second fib time = " );

    cout << "Time for 3 fib ... " << endl;
    y.start();
    x.start(); // resume
    fib( 43 );
    x.stop();
    x.print( "", Stopwatch::MICROSEC );
    x.print( "", Stopwatch::MILLISEC );
    x.print();
    x.print( "", Stopwatch::MINUTES );
    x.print( "", Stopwatch::HOURS );
    x.print( "", Stopwatch::DAYS, cerr );
    y.stop();

    x.reset();
    x.start(); // start from new
    fib( 43 );
    x.stop();
    x.print( "Last fib time = " );
  }
  {
    ScopeTimer ot( "outside for-loop" );
    for ( int32_t i = 0; i < 5; ++i )
    {
      ScopeTimer t( "in for-loop" );
      y.start();
      usleep( 831234 ); // ... do computations for 5.83 sec each
      y.stop();
    }
  }
  y.print( "Hi" );
  {
    SeriesTimer x;
    for ( int32_t i = 0; i < 10; ++i )
    {
      x.start();
      // do computation for about 1 sec ...
      usleep( 1000236 );
      x.stop();
    }
    x.print( "Timings: " );
  }
  return 0;
}
