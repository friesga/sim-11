#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include <string>
#include <mutex>
#include <time.h>
#include <stdio.h>

// A very basic thread-safe Logger class
class Logger
{
private:
    static std::ofstream logFile_;
    static std::mutex logMutex_;
    static Logger logger_;

    const std::string currentDateTime ();

protected:
    // Prevent instantation of the class
    Logger () {};

public:
    ~Logger ();

    static void init (std::string fileName);
    static Logger &instance ();

    template <typename T>
    Logger &operator<< (T objectToLog);
};

// Insertion operator
// Log messages are appended with a newline to flush the buffer immediately
template <typename T>
Logger &Logger::operator<< (T objectToLog)
{
    if (logFile_.is_open ())
    {
        // Guard against log file writes from different threads
	    std::lock_guard<std::mutex> guard{ logMutex_ };

        logFile_ << "[ INFO ][ " << currentDateTime () << "]: " << 
            objectToLog << std::endl;
    }
    return logger_;
}

#endif // _LOGGER_H_