// Platform specific file open function
// 
// For Linux use 64 bit (Large File Support) functions.

#include "sim_fio.h"

FILE *sim_fopen (const char *file, const char *mode)
{
#if (defined (__linux) || defined (__linux__))
    return fopen64 (file, mode);
#else
    return fopen (file, mode);
#endif
}