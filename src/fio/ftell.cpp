#include "fio.h"

#if defined (_WIN32)

#include <sys/stat.h>

t_offset fio::ftell (FILE* st)
{
    return (t_offset) _ftelli64 (st);
}

#endif 

#if defined (__linux) || defined (__linux__) 

t_offset fio::ftell (FILE* st)
{
    return (t_offset) ftello64 (st);
}

#endif

#if (__EMSCRIPTEN__)

t_offset fio::ftell (FILE* st)
{
    return std::ftell (st);
}

#endif // 
