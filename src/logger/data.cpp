#include "logger.h"

std::ofstream Logger::logFile_;
std::mutex Logger::logMutex_;
Logger Logger::logger_;