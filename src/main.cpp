#include <iostream>

#include "scopetimer.hpp"
#include "stopwatch.hpp"
#include "seriestimer.hpp"
#include <unistd.h>

using namespace std;
using namespace timer;

unsigned long fib(size_t n) {
    //ScopeTimer f("inFib");
    if (n == 0)
        return 0;
    if (n == 1)
        return 1;
    return fib(n-1) + fib(n-2);
}

int main (int argc, char const *argv[])
{
    SeriesTimer y;
#pragma omp parallel num_threads(4)
    {
        ScopeTimer f("fib");
        Stopwatch x;
        

        y.start();
        x.start();
        fib(43);
        x.stop();
        y.stop();
        
        x.print("First fib time = ");
        
        y.start();
        x.start(); // resume
        fib(43);
        x.print("Some intermediate: ");
        x.stop();
        y.stop();
        
        x.print("First & Second fib time = ");
        
        cout << "Time for 3 fib ... " << endl;
        y.start();
        x.start(); // resume
        fib(43);
        x.stop();
        x.print("", Stopwatch::MICROSEC);
        x.print("", Stopwatch::MILLISEC);
        x.print();
        x.print("", Stopwatch::MINUTES);
        x.print("", Stopwatch::HOURS);
        x.print("", Stopwatch::DAYS, cerr);
        y.stop();
        
        x.reset();
        x.start(); // start from new
        fib(43);
        x.stop();
        x.print("Last fib time = ");
    }
    {
        ScopeTimer ot("outside for-loop");
        for (int i = 0; i < 5; ++i)
        {
            ScopeTimer t("in for-loop");
            y.start();
            usleep(831234);// ... do computations for 5.83 sec each
            y.stop();
        }
    }
    y.print("Hi");
    {
        SeriesTimer x;
        for (int i = 0; i < 10; ++i)
        {
            x.start();
            // do computation for about 1 sec ...
            usleep(1000236);
            x.stop();
        }
        x.print("Timings: ");
    }
    return 0;
}