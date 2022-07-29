#include "rlv12.h"

inline int32_t RLV12::getCylinder (int32_t track)
{
    return (track >> RLDA_V_CYL) & RLDA_M_CYL;
}