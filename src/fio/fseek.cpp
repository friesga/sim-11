#include "fio.h"

#if defined (_WIN32)

#include <sys/stat.h>

int fio::fseek (FILE * st, t_offset offset, int origin)
{
    return _fseeki64 (st, offset, origin);
}

#endif 


#if defined (__linux) || defined (__linux__) 

int fio::fseek (FILE* st, t_offset xpos, int origin)
{
    return fseeko64 (st, (off64_t)xpos, origin);
}

#endif