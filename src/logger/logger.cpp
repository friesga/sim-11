#include "logger.h"

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

// Get current date/time in the format DD-MM-YYY HH:mm:ss
const std::string Logger::currentDateTime()
{
    time_t     now = time (0);
    struct tm  tstruct;
    char       buf[80];

    localtime_s (&tstruct, &now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime for more
    // information about date/time format
    strftime(buf, sizeof(buf), "%d-%b-%Y %H:%M:%S", &tstruct);

    return buf;
}