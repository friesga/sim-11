#include "sim_fio.h"

// ToDo: Calculate littleEndian variable (see finit)
bool const littleEndian = true;

size_t sim_fread (void* bptr, size_t size, size_t count, FILE* fptr)
{
    size_t c, j;
    size_t k;
    unsigned char by, *sptr, *dptr;

    // Check arguments
    if ((size == 0) || (count == 0))                        
        return 0;

    // Read buffer
    c = fread(bptr, size, count, fptr);

    // If we are on a litlle endian machine or reading just one byte or we
    // encountered an error, we're done.
    if (littleEndian || (size == sizeof(char)) || (c == 0))
        return c;

    // Convert to big endian
    for (j = 0, dptr = sptr = (unsigned char*) bptr; j < c; j++)
    { 
        // For all items, swap end-for-end
        for (k = size - 1; k >= ((size + 1) / 2); --k)
        {
            by = *sptr;
            *sptr++ = *(dptr + k);
            *(dptr + k) = by;
        }
        sptr = dptr = dptr + size;
    }
    return c;
}