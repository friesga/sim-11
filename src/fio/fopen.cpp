#include "fio.h"

// For Linux use 64 bit (Large File Support) functions.
FILE* fio::fopen (const char* file, const char* mode)
{
#if (defined (__linux) || defined (__linux__))
    return std::fopen64 (file, mode);
#else
    return std::fopen (file, mode);
#endif
}