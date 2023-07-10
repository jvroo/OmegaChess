#ifndef UCITIMER_H
#define UCITIMER_H

/** \file UCITimer.h
    \brief Contains multiplatform timer with UCI Interface
 */

#ifdef WIN64
    #include <windows.h>
#else
    #include <sys/time.h>
#endif // WIN64

//NOTE NOT YET TESTED ON LINUX OR MAC OS
class UCITimer
{
public:
    //Public functions
    static int get_time_ms();
    static int input_waiting();
    static void read_input();
    static void communicate();
    static bool get_stopped();
    static void set_stopped(bool is_stopped);
    static int movestogo;
    static int movetime;
    static int time;
    static int inc;
    static int starttime;
    static int stoptime;
    static int timeset;
private:
    //UCI Timing Private Variables 
    static bool quit;
    static bool stopped;
};

#endif  
    
    
    
    