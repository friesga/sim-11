#include "fio.h"

size_t fio::fread (void* bptr, size_t size, size_t count, FILE* fptr)
{
    return std::fread (bptr, size, count, fptr);
}