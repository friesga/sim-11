#include "sim_fio.h"

// Windows 

#if defined (_WIN32)
#define S_SIM_IO_FSEEK_EXT_ 1
#include <sys/stat.h>

/* [JDB] The previous Win32 versions of sim_fseeko and sim_ftell attempted to
   use fsetpos and fgetpos by manipulating an fpos_t value as though it were a
   64-bit integer.  This worked with version 5.0 of the mingw runtime library,
   which declared an fpos_t to be a long long int.  With version 5.2, fpos_t is
   now a union containing a long long int, so that it cannot be manipulated.
   The manipulation was always suspect, as the MSVC++ 2008 documentation says,
   "The [fpos_t] value is stored in an internal format and is intended for use
   only by fgetpos and fsetpos."  It worked, but only because VC++ declared it
   as an __int64 value.  If that changes, the original code would break, as it
   now does for mingw.

   Therefore, we now simply call _fseeki64 and _ftelli64, which are provided by
   both mingw and VC++ and work as expected without manipulation.
*/

int sim_fseek (FILE *st, t_offset offset, int whence)
{
    return _fseeki64 (st, offset, whence);
}

t_offset sim_ftell (FILE *st)
{
    return (t_offset) _ftelli64 (st);
}

#endif 

// Linux

#if defined (__linux) || defined (__linux__) 
#define S_SIM_IO_FSEEK_EXT_ 1

int sim_fseek (FILE *st, t_offset xpos, int origin)
{
    return fseeko64 (st, (off64_t)xpos, origin);
}

t_offset sim_ftell (FILE *st)
{
    return (t_offset)(ftello64 (st));
}

#endif 

