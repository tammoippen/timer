/**
 * scopetimer.hpp
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

#ifndef SCOPETIMER_H
#define SCOPETIMER_H

#include <string>

#include "stopwatch.hpp"
#include "timer_config.hpp"

namespace timer
{
/********************************************************************
 * ScopeTimer                                                       *
 *   Accumulates elapsed times between creation and                 *
 *   destruction of objects with the same 'name'.                   *
 *                                                                  *
 *   Usage example:                                                 *
 *   { // start of some scope                                       *
 *     ScopeTimer ot("outside for-loop");                           *
 *     for (int i = 0; i < 5; ++i)                                  *
 *     {                                                            *
 *       ScopeTimer t("in for loop");                               *
 *       // ... do computations for 5.83 sec each                   *
 *     }                                                            *
 *   } // end of some scope                                         *
 *   // ... other stuff                                             *
 *   // output at program exit:                                     *
 *   Collected Timers for thread  0                                 *
 *                  in for-loop (calls    5) :: 29.156 sec.         *
 *             outside for-loop (calls    1) :: 29.1562 sec.        *
 ********************************************************************/
class ScopeTimer
{
public:
    /**
     * Creates a ScopeTimer and starts the stopwatch.
     */
    ScopeTimer(const std::string& name);

    /**
     * Before destroying the timer, it stops the stopwatch and registers
     * the elapsed time + its name to a global collector.
     */
    ~ScopeTimer();

private:
#ifdef ENABLE_TIMING
    std::string _name;
    Stopwatch _stopwatch;
#endif
};

} /* namespace  */

#endif /* SCOPETIMER_H */
