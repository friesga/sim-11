#ifndef _FIO_H_
#define _FIO_H_

#include <cstdio>
#include <cstdint>

// This namespace provides as set of functions that are a thin layer on top
// of the C stream functions. It can be  used interchangeably with these
// functions. The functions provide the following features:
// - Abstracts some Windows/Linux differences for large file support,
// - Adds a function returning the size of a file.
//
// These functions assume they run on a little endian machine. If big endian
// support is needed this could be the place to add support for that
// endianness. As big endian machines are rare these days and the code
// couldn't be tested, support for that type of machine isn't included.
// 

// Assuming all supported systems provide large file support
using t_offset = uint64_t;

namespace fio
{
    FILE* fopen (const char* file, const char* mode);
    size_t fread (void* bptr, size_t size, size_t count, FILE* fptr);
    size_t fwrite (void* bptr, size_t size, size_t count, FILE* fptr);
    int fseek (FILE* st, t_offset offset, int origin);
    t_offset ftell (FILE* st);
    t_offset fsize (FILE* fp);
};

#endif // _FIO_H_