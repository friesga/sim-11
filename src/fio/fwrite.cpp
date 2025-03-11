#include "fio.h"

size_t fio::fwrite (void* bptr, size_t size, size_t count, FILE* fptr)
{
    return std::fwrite (bptr, size, count, fptr);
}