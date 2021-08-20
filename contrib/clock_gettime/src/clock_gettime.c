
#include <Windows.h>
#include <time.h>

//C-file part
int clock_gettime (int clock, struct timespec* spec)     
{
    __int64 wintime;
    GetSystemTimeAsFileTime ((FILETIME*) &wintime);
    wintime -= 116444736000000000i64;               //1jan1601 to 1jan1970
    spec->tv_sec = wintime / 10000000i64;           //seconds
    spec->tv_nsec = wintime % 10000000i64 * 100;    //nano-seconds
    return 0;
}