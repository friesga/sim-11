#include "fio.h"

// return file size
t_offset fio::fsize (FILE* fp)
{
    t_offset currentPosition, size;

    if (fp == NULL)
        return 0;

    currentPosition = fio::ftell (fp);

    // Seek to the end of the file. That position tells us the size of the
    // file.
    fio::fseek (fp, 0, SEEK_END);
    size = fio::ftell (fp);

    // Finally return to the original position in the file
    fio::fseek (fp, currentPosition, SEEK_SET);

    return size;
}