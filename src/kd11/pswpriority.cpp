#include "kd11.h"

u8 KD11CPU::pswPriority()
{
    return (psw & PSW_PRIORITY) >> 5;
}