/*
 *  scopetimer.h
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

#ifndef SCOPETIMER_H
#define SCOPETIMER_H

#include "stopwatch.h"
#include <string>

namespace nest
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

} /* namespace nest */

#endif /* SCOPETIMER_H */