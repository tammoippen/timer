/**
 * stopwatch.cpp
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

#include "stopwatch.hpp"

#include <cassert>
#include <sys/time.h>

namespace timer
{
std::ostream&
operator<<( std::ostream& os, const Stopwatch& stopwatch )
{
  stopwatch.print( "", Stopwatch::SECONDS, os );
  return os;
}
}

timer::Stopwatch::Stopwatch()
{
  reset();
}

void
timer::Stopwatch::start()
{
#ifdef ENABLE_TIMING
  if ( not isRunning() )
  {
    _prev_elapsed += _end - _beg;  // store prev. time, if we resume
    _end = _beg = get_timestamp(); // invariant: _end >= _beg
    _running = true;               // we start running
  }
#endif
}

void
timer::Stopwatch::stop()
{
#ifdef ENABLE_TIMING
  if ( isRunning() )
  {
    _end = get_timestamp(); // invariant: _end >= _beg
    _running = false;       // we stopped running
  }
#endif
}

bool
timer::Stopwatch::isRunning() const
{
#ifdef ENABLE_TIMING
  return _running;
#else
  return false;
#endif
}

double
timer::Stopwatch::elapsed( timeunit_t timeunit ) const
{
#ifdef ENABLE_TIMING
  assert( correct_timeunit( timeunit ) );
  return 1.0 * elapsed_timestamp() / timeunit;
#else
  return 0.0;
#endif
}

timer::Stopwatch::timestamp_t
timer::Stopwatch::elapsed_timestamp() const
{
#ifdef ENABLE_TIMING
  if ( isRunning() )
  {
    // get intermediate elapsed time; do not change _end, to be const
    return get_timestamp() - _beg + _prev_elapsed;
  }
  else
  {
    // stopped before, get time of current measurment + last measurments
    return _end - _beg + _prev_elapsed;
  }
#else
  return ( timestamp_t ) 0;
#endif
}

void
timer::Stopwatch::reset()
{
#ifdef ENABLE_TIMING
  _beg = 0; // invariant: _end >= _beg
  _end = 0;
  _prev_elapsed = 0; // erase all prev. measurments
  _running = false;  // of course not running.
#endif
}

void
timer::Stopwatch::print( const char* msg, timeunit_t timeunit, std::ostream& os ) const
{
#ifdef ENABLE_TIMING
  assert( correct_timeunit( timeunit ) );
  double e = elapsed( timeunit );
  os << msg << e;
  switch ( timeunit )
  {
    case MICROSEC:
      os << " microsec.";
      break;
    case MILLISEC:
      os << " millisec.";
      break;
    case SECONDS:
      os << " sec.";
      break;
    case MINUTES:
      os << " min.";
      break;
    case HOURS:
      os << " h.";
      break;
    case DAYS:
      os << " days.";
      break;
  }
#ifndef NDEBUG
  os << " (running: " << ( _running ? "true" : "false" ) << ", begin: " << _beg << ", end: " << _end
     << ", diff: " << ( _end - _beg ) << ", prev: " << _prev_elapsed << ")";
#endif
  os << std::endl;
#endif
}

timer::Stopwatch::timestamp_t
timer::Stopwatch::get_timestamp()
{
  struct timeval now;
  gettimeofday( &now, ( struct timezone* ) 0 );
  return ( timer::Stopwatch::timestamp_t ) now.tv_usec
    + ( timer::Stopwatch::timestamp_t ) now.tv_sec * timer::Stopwatch::SECONDS;
}
