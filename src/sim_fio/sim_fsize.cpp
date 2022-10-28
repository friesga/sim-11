#include "sim_fio.h"

// Get file size

t_offset sim_fsize (FILE* fp)
{
    t_offset position, size;

    if (fp == NULL)
        return 0;

    position = sim_ftell (fp);
    sim_fseek (fp, 0, SEEK_END);

    size = sim_ftell (fp);
    sim_fseek (fp, position, SEEK_SET);

    return size;
}