/*
 *  scopetimer.cpp
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

#include "scopetimer.h"
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <map>

#ifdef _OPENMP
#include <omp.h>
#endif

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
#ifdef ENABLE_TIMING
class ScopeTimeCollector
{
private:
    /**
     * Holds the data for ScopeTimer with same name
     * and on the same thread.
     */
    struct SScopeData
    {
        double dTime;
        uint64_t lCall;

        SScopeData& update(double time)
        {
            dTime += time;
            ++lCall;
            return *this;
        }
    };

    typedef std::map<std::string, SScopeData> mapping;

    mapping *_timing_data;
    uint64_t _threads;

#ifdef _OPENMP
    omp_lock_t   globalLock;
#endif

public:
    ScopeTimeCollector()
    {
#ifdef _OPENMP
        _threads = omp_get_max_threads();
        omp_init_lock(&globalLock);
#else
        _threads = 1;
#endif
        _timing_data = new mapping[_threads];
    }

    ~ScopeTimeCollector()
    {
        mapping::iterator it;
        // foreach thread
        for (size_t i = 0 ; i < _threads ; i++)
        {
            // if thread contains data
            if (_timing_data[i].size() > 0)
            {
                std::cerr << std::endl << "\nCollected Timers for thread ";
                std::cerr << std::setw(2) << i << std::endl;
                // output all timing data
                for(it=_timing_data[i].begin(); 
                    it != _timing_data[i].end(); 
                    it++)
                {
                    std::cerr << std::setw(30) << it->first.c_str()
                              << " (calls " 
                              << std::setw(4) << it->second.lCall
                              << ") :: " 
                              << std::setw(18) << it->second.dTime
                              << " sec." << std::endl;
                }
            }
        }
        delete[] _timing_data;
#ifdef _OPENMP
        omp_destroy_lock(&globalLock);
#endif
    }

    /**
     * Register/ add a measurement of a certain name.
     */
    void add(const std::string& name, double time)
    {
#ifdef _OPENMP
        omp_set_lock(&globalLock);
        uint64_t tid = omp_get_thread_num();
        _timing_data[tid][name] = _timing_data[tid][name].update(time);
        omp_unset_lock(&globalLock);
#else
        _timing_data[0][name] = _timing_data[0][name].update(time);
#endif
    }
};
/** global instance of the ScopeTimeCollector **/
ScopeTimeCollector scopetimecollector;
#endif /* ENABLE_TIMING */

nest::ScopeTimer::ScopeTimer(const std::string& name)
#ifdef ENABLE_TIMING
: _name(name), _stopwatch()
{
    _stopwatch.start();
}
#else
{}
#endif

nest::ScopeTimer::~ScopeTimer()
{
#ifdef ENABLE_TIMING
    _stopwatch.stop();
    scopetimecollector.add(_name, _stopwatch.elapsed(Stopwatch::SECONDS));
#endif
}
