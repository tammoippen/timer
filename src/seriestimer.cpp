/*
 *  seriestimer.cpp
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "seriestimer.h"
#include <cassert>
#include <algorithm>
#include <cmath>

namespace nest
{
    std::ostream& operator<<(std::ostream& os, 
                         const SeriesTimer& seriestimer) 
    {
        seriestimer.print("", Stopwatch::SECONDS, os);
        return os;
    }
}

nest::SeriesTimer::SeriesTimer()
#ifdef ENABLE_TIMING
: _stopwatch(), _timestamps()
#endif
{
}

void nest::SeriesTimer::start()
{
#ifdef ENABLE_TIMING
    _stopwatch.start();
#endif
}

void nest::SeriesTimer::stop()
{
#ifdef ENABLE_TIMING
    _stopwatch.stop();
    _timestamps.push_back(_stopwatch.elapsed_timestamp());
    _stopwatch.reset();
#endif
}

bool nest::SeriesTimer::isRunning() const
{
#ifdef ENABLE_TIMING
    return _stopwatch.isRunning();
#endif
}

void nest::SeriesTimer::reset()
{
#ifdef ENABLE_TIMING
    _stopwatch.reset();
    _timestamps.clear();
#endif
}

std::vector<double> nest::SeriesTimer::timings(Stopwatch::timeunit_t timeunit) const
{
    #ifdef ENABLE_TIMING
    assert(Stopwatch::correct_timeunit(timeunit));
    std::vector<double> result(_timestamps.size());
    // convert to vector of requestet timeunit
    for (int i = 0; i < _timestamps.size(); ++i)
    {
        result[i] = 1.0 * _timestamps[i] / timeunit;
    }

    return result;
#else
    std::vector<double> v;
    return v;
#endif
}

double nest::SeriesTimer::sum(Stopwatch::timeunit_t timeunit) const
{
#ifdef ENABLE_TIMING
    assert(Stopwatch::correct_timeunit(timeunit));
    double sum = 0.0;
    for (int i = 0; i < _timestamps.size(); ++i)
    {
        sum += _timestamps[i];
    }
    return sum / timeunit;
#else
    return 0.0;
#endif
}

double nest::SeriesTimer::mean(Stopwatch::timeunit_t timeunit) const
{
#ifdef ENABLE_TIMING
    assert(Stopwatch::correct_timeunit(timeunit));
    if (! _timestamps.empty())
    {
        return sum(timeunit) / _timestamps.size();
    }
    else
    {
        return 0.0;
    }
#else
    return 0.0;
#endif
}

double nest::SeriesTimer::std(Stopwatch::timeunit_t timeunit) const
{
#ifdef ENABLE_TIMING
    assert(Stopwatch::correct_timeunit(timeunit));
    double r = mean(timeunit);
    double sum = 0;
    double tmp;

    for (int i = 0; i < _timestamps.size(); ++i)
    {
        tmp = 1.0 * _timestamps[i] / timeunit - r; // difference
        sum += tmp * tmp; // squaring
    }
    // sqrt of sum of squared differences
    return std::sqrt(sum / _timestamps.size());
#else
    return 0.0;
#endif
}

double nest::SeriesTimer::quantile(double q, Stopwatch::timeunit_t timeunit) const
{
#ifdef ENABLE_TIMING
    assert(Stopwatch::correct_timeunit(timeunit));
    assert(q >= 0.0); // not smaller than min
    assert(q <= 1.0); // not larger than max
    std::vector<Stopwatch::timestamp_t> local = _timestamps;

    // quantiles need sorting
    std::sort(local.begin(), local.end());
    // select the index of quantile; in doubt select smaller one
    int i = (int) std::ceil(q * local.size()) - 1;
    if (i < 0) // if 0 is choosen, the calculation is -1
    {
        i = 0;
    }
    return 1.0 * local[i] / timeunit; // return correct timeunit
#else
    return 0.0;
#endif
}

void nest::SeriesTimer::print(const char* msg, Stopwatch::timeunit_t timeunit, 
               std::ostream& os) const
{
#ifdef ENABLE_TIMING
    assert(Stopwatch::correct_timeunit(timeunit));
    
    os << msg;
    switch (timeunit)
    {
        case Stopwatch::MICROSEC: os << "(microsec) ["; break;
        case Stopwatch::MILLISEC: os << "(millisec) ["; break;
        case Stopwatch::SECONDS:  os << "(sec) ["; break;
        case Stopwatch::MINUTES:  os << "(min) ["; break;
        case Stopwatch::HOURS:    os << "(h) ["; break;
        case Stopwatch::DAYS:     os << "(days) ["; break;
        default: return;
    }
    std::vector<double> t = timings(timeunit);
    for (int i = 0; i < t.size(); ++i)
    {
        if (i != 0)
        {
            os << ", "; 
        }
        os << t[i];
    }
    os << " ] " << std::endl;

    os << "Statistics: "<< std::endl;
    os << "              sum = " << sum(timeunit) << std::endl;
    os << "             mean = " << mean(timeunit) << std::endl;
    os << "              std = " << std(timeunit) << std::endl;
    os << "      q 0\% (min) = " << quantile(0.0, timeunit) << std::endl;
    os << "           q 25\% = " << quantile(0.25, timeunit) << std::endl;
    os << "  q 50\% (median) = " << quantile(0.5, timeunit) << std::endl;
    os << "           q 75\% = " << quantile(0.75, timeunit) << std::endl;
    os << "    q 100\% (max) = " << quantile(1.0, timeunit) << std::endl;
#endif
}