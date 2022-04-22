#include "kd11.h"

u8 KD11CPU::cpuPriority()
{
    return (psw & PSW_PRIORITY) >> 5;
}