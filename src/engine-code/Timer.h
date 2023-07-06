#ifndef TIMER_H
#define TIMER_H

/** \file Timer.h
    \brief Contains multiplatform timer
 */

#ifdef WIN64
    #include <windows.h>
#else
    #include <sys/time.h>
#endif // WIN64

//NOTE NOT YET TESTED ON LINUX OR MAC OS
class Timer
{
public:
    static void start();
    static void stop();
    static void reset();
    static void display();
    static int readTime();
private:
    static int get_time_milliseconds();
    static int startTime;
    static int endTime;
    static int elapsed;
    static bool paused;
};

#endif


