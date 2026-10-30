#include "rlv12.h"

// CRC16 as implemented by the DEC 9401 chip 
u16 RLV12::calcCRC(const int wc, const u16* data)
{
    u32  crc, j, d;
    int32_t   i;

    crc = 0;
    for (i = 0; i < wc; ++i)
    {
        d = *data++;
        // Cribbed from KG11-A 
        for (j = 0; j < 16; j++)
        {
            crc = (crc & ~01) | ((crc & 01) ^ (d & 01));
            crc = (crc & 01) ? (crc >> 1) ^ 0120001 : crc >> 1;
            d >>= 1;
        }
    }
    return static_cast <u16> (crc);
}