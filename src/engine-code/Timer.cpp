#include "Timer.h"
#include <iostream>
using namespace std;

int Timer::get_time_milliseconds()
{
    #ifdef WIN64
        return GetTickCount();
    #else
        struct timeval time_value;
        gettimeofday(&time_value, NULL);
        return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
    #endif // WIN64
}

int Timer::startTime{0};
int Timer::endTime{0};
int Timer::elapsed{0};
bool Timer::paused{true};

//Resets the elapsed time to zero and pauses timer
void Timer::reset()
{
    elapsed = 0;
    paused = true;
}

//The start button of the timer to unpause
void Timer::start()
{
    if (paused)
    {
        startTime = get_time_milliseconds();
        paused = false;
    }
}

//The stop button of the timer to pause
void Timer::stop()
{
    if (!paused)
    {
        endTime = get_time_milliseconds();
        elapsed = elapsed + (endTime - startTime);
        paused = true;
    }
}


//Displays the recorded elapsed time of the timer
void Timer::display()
{
    if (paused)
    {
         cout << "Total time is: " << elapsed << " milliseconds" <<endl;
    }
}

//Returns the recorded elapsed time of the timer
int Timer::readTime()
{
    if (paused)
    {
        return elapsed;
    }
    return 0;
}
