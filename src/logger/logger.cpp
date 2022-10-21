#include "logger.h"

#include <iomanip>
#include <sstream>
#include <stdlib.h>

// Destructor
Logger::~Logger ()
{
    logFile_.close ();
}

void Logger::init (std::string fileName)
{
    logFile_.open (fileName);
}

// Get a reference to the one and only Logger object
Logger &Logger::instance ()
{
    return logger_;
}

// Hide the difference between POSIX and MSC thread-safe localtime versions.
// 
inline std::tm Logger::localtime_xp (std::time_t timer)
{
    std::tm bt {};

#if defined(__unix__)
    localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
    localtime_s(&bt, &timer);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    bt = *std::localtime(&timer);
#endif
    return bt;
}

// Get current date/time in the format DD-MM-YYY HH:mm:ss
const std::string Logger::currentDateTime()
{
    time_t now = time (0);
    struct tm tstruct;
    char buf[80];

    tstruct = localtime_xp (now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime for more
    // information about date/time format
    strftime(buf, sizeof(buf), "%d-%b-%Y %H:%M:%S", &tstruct);

    return buf;
}

