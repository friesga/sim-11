#include "sim_fio.h"

// littleEndian is defined in sim_fread.cpp
extern bool const littleEndian;

#define FLIP_SIZE       (1 << 16)
static unsigned char sim_flip[FLIP_SIZE];

size_t sim_fwrite(void* bptr, size_t size, size_t count, FILE* fptr)
{
    size_t c, j, nelem, nbuf, lcnt, total;
    int32_t i, k;
    unsigned char* sptr, * dptr;

    // Check arguments
    if ((size == 0) || (count == 0))
        return 0;

    // If writing on a little endian machine or writing just once char
    // a call to fwrite() suffices
    if (littleEndian || (size == sizeof(char)))
        return fwrite(bptr, size, count, fptr);

    // Convert buffer for big endian
    nelem = FLIP_SIZE / size;            // Elements in buffer */
    nbuf = count / nelem;                // number buffers 
    lcnt = count % nelem;                // Count in last buf
    if (lcnt)
        nbuf = nbuf + 1;
    else
        lcnt = nelem;
    total = 0;

    sptr = (unsigned char*)bptr;         // Init input ptr
    for (i = nbuf; i > 0; i--)
    {                           
        // Loop on buffers
        c = (i == 1) ? lcnt : nelem;
        for (j = 0, dptr = sim_flip; j < c; j++)
        {          
            // Loop on items
            for (k = size - 1; k >= 0; k--)
                *(dptr + k) = *sptr++;
            dptr = dptr + size;
        }
        c = fwrite(sim_flip, size, c, fptr);
        if (c == 0)
            return total;
        total = total + c;
    }
    return total;
}