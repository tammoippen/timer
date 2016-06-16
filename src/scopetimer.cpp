/**
 * scopetimer.cpp
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

#include "scopetimer.hpp"

#include <iomanip>
#include <iostream>
#include <map>
#include <stdint.h>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace timer
{
/***********************************************************************
 * ScopeTimeCollector                                                  *
 *   Collects the accumulated times of the named ScopeTimer in         *
 *   the background. Whenever a ScopeTimer is deleted, it registers    *
 *   its name and accumulated time with the collector.                 *
 *   Additionally, the collector stores the number of occurences.      *
 *   If compiled with OpenMP, the collector assignes the ScopeTimer    *
 *   data to the corresponding thread.                                 *
 *                                                                     *
 *   When the ScopeTimeCollector is deleted itself (most likely at     *
 *   the end of the program), it outputs the collected data to the     *
 *   std:cerr stream.                                                  *
 ***********************************************************************/
#if defined( ENABLE_TIMING ) && defined( ENABLE_SCOPETIMER )
class ScopeTimeCollector
{
private:
  /**
   * Holds the data for ScopeTimer with same name
   * and on the same thread.
   */
  struct SScopeData
  {
    double time;
    uint64_t num_calls;

    SScopeData&
    update( double time )
    {
      this->time += time;
      ++num_calls;
      return *this;
    }
  };

  typedef std::map< std::string, SScopeData > mapping;

  mapping* _timing_data;
  uint64_t _threads;
  Stopwatch _sw_overall;

#ifdef _OPENMP
  omp_lock_t globalLock;
#endif

  // C++ 03
  // ========
  // Dont forget to declare these two. You want to make sure they
  // are unacceptable otherwise you may accidentally get copies of
  // your singleton appearing.
  ScopeTimeCollector( ScopeTimeCollector const& ); // Don't Implement
  void operator=( ScopeTimeCollector const& );     // Don't implement

public:
  ScopeTimeCollector()
  {
#ifdef _OPENMP
    _threads = omp_get_max_threads();
    omp_init_lock( &globalLock );
#else
    _threads = 1;
#endif
    _timing_data = new mapping[ _threads ];
    _sw_overall.start();
  }

  ~ScopeTimeCollector()
  {
    mapping::iterator it;
    // foreach thread
    for ( size_t i = 0; i < _threads; i++ )
    {
      // if thread contains data
      if ( _timing_data[ i ].size() > 0 )
      {
        std::cerr << std::endl << "\nCollected Timers for thread ";
        std::cerr << std::setw( 2 ) << i << std::endl;
        // output all timing data
        for ( it = _timing_data[ i ].begin(); it != _timing_data[ i ].end(); ++it )
        {
          std::cerr << std::setw( 30 ) << it->first.c_str() << " (calls " << std::setw( 4 )
                    << it->second.num_calls << ") :: " << std::setw( 18 ) << it->second.time
                    << " sec." << std::endl;
        }
      }
    }
    delete[] _timing_data;
#ifdef _OPENMP
    omp_destroy_lock( &globalLock );
#endif
    _sw_overall.stop();
    _sw_overall.print( "Complete execution took " );
  }

  /**
   * Register/ add a measurement of a certain name.
   */
  void
  add( const std::string& name, double time )
  {
#ifdef _OPENMP
    omp_set_lock( &globalLock );
    uint64_t tid = omp_get_thread_num();
    _timing_data[ tid ][ name ] = _timing_data[ tid ][ name ].update( time );
    omp_unset_lock( &globalLock );
#else
    _timing_data[ 0 ][ name ] = _timing_data[ 0 ][ name ].update( time );
#endif
  }
};
/** global instance of the ScopeTimeCollector **/
ScopeTimeCollector scopetimecollector;
#endif /* #if defined( ENABLE_TIMING ) && defined( ENABLE_SCOPETIMER ) */
}

#if defined( ENABLE_TIMING ) && defined( ENABLE_SCOPETIMER )
timer::ScopeTimer::ScopeTimer( const std::string& name )
  : _name( name )
  , _stopwatch()
{
  _stopwatch.start();
}
#else
timer::ScopeTimer::ScopeTimer( const std::string& )
{
}
#endif

timer::ScopeTimer::~ScopeTimer()
{
#if defined( ENABLE_TIMING ) && defined( ENABLE_SCOPETIMER )
  _stopwatch.stop();
  scopetimecollector.add( _name, _stopwatch.elapsed( Stopwatch::SECONDS ) );
#endif
}
