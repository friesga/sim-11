#include "fio.h"

#include <stdio.h>      // Required for fopen64()

// For Linux use 64 bit (Large File Support) functions.
FILE* fio::fopen (const char* file, const char* mode)
{
#if (defined (__linux) || defined (__linux__))
    return fopen64 (file, mode);
#else
    return std::fopen (file, mode);
#endif
}