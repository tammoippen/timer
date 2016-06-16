/*
 *  seriestimer.h
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

#ifndef SERIES_TIMER_H
#define SERIES_TIMER_H

#include <stdint.h>
#include <iostream>
#include <vector>
#include "stopwatch.h"

namespace nest
{

/************************************************************************
 * SeriesTimer                                                          *
 *   The SeriesTimer stores subsequent timings in a vector and is       *
 *   able to perform basic statistical analysis on the time series.     *
 *                                                                      *
 *   Not thread-safe: - Do not share SeriesTimer among threads.         *
 *                    - Let each thread have its own SeriesTimer.       *
 *                                                                      *
 *   Usage example:                                                     *
 *     SeriesTimer x;                                                   *
 *     for (int i = 0; i < 10; ++i)                                     *
 *     {                                                                *
 *         x.start();                                                   *
 *         // do computation for about 1 sec ...                        *
 *         x.stop();                                                    *
 *     }                                                                *
 *     x.print("Timings: ");                                            *
 *     //  Timings: (sec) [1.00134, 1.00134, 1.00134, 1.00137, 1.00134, *
 *     //                  1.00095, 1.00134, 1.00136, 1.00064, 1.00134 ]*
 *     //  Statistics:                                                  *
 *     //               sum = 10.0124                                   *
 *     //              mean = 1.00124                                   *
 *     //               std = 0.000231707                               *
 *     //        q 0% (min) = 1.00064                                   *
 *     //             q 25% = 1.00134                                   *
 *     //    q 50% (median) = 1.00134                                   *
 *     //             q 75% = 1.00134                                   *
 *     //      q 100% (max) = 1.00137                                   *
 *     cout << "Sum: " << x.sum(Stopwatch::SECONDS) << " sec." << endl; *
 *     cout << "Avg: " << x.mean() << " sec." << endl; // default is sec*
 *     cout << "Std: " << x.std() << " sec." << endl;                   *
 *     x.reset(); // clear the timings                                  *
 ************************************************************************/
class SeriesTimer
{
public:
    
    /** 
     * Creates a SeriesTimer that is not running.
     */
    SeriesTimer();
    
    /** 
     * Beginns a new measurment for this SeriesTimer.
     */
    void start();
    
    /** 
     * Stops the SeriesTimer and stores the resulting time.
     */
    void stop();

    /**
     * Returns, whether the SeriesTimer is running.
     */
    bool isRunning() const;

    /** 
     * Resets the SeriesTimer.
     */
    void reset();

    /** 
     * Returns the individual timings in the requested timeunit.
     */
    std::vector<double> timings(Stopwatch::timeunit_t timeunit = Stopwatch::SECONDS) const;

    /**
     * Returns the total elapsed time of the series.
     */
    double sum(Stopwatch::timeunit_t timeunit = Stopwatch::SECONDS) const;

    /**
     * Returns the average time of the series.
     */
    double mean(Stopwatch::timeunit_t timeunit = Stopwatch::SECONDS) const;

    /**
     * Returns the standard deviation time of the series.
     */
    double std(Stopwatch::timeunit_t timeunit = Stopwatch::SECONDS) const;

    /**
     * Returns the q-th quantile timing of the series.
     */
    double quantile(double q = 0.5, Stopwatch::timeunit_t timeunit = Stopwatch::SECONDS) const;
    
    /** 
     * This method prints out the currently elapsed time.
     */
    void print(const char* msg = "", Stopwatch::timeunit_t timeunit = Stopwatch::SECONDS, 
               std::ostream& os = std::cout) const;
    
    /**
     * Convenient method for writing time in seconds
     * to some ostream.
     */
    friend std::ostream& operator<< (std::ostream& os, 
                                     const SeriesTimer& SeriesTimer);

private:
#ifdef ENABLE_TIMING
    Stopwatch _stopwatch;
    std::vector<Stopwatch::timestamp_t>  _timestamps;
#endif
};

} /* namespace timer */
#endif /* SERIES_TIMER_H */
