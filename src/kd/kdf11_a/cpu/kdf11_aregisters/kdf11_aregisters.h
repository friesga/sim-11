#ifndef _KDF11_AREGISTERS_H_
#define _KDF11_AREGISTERS_H_

#include "kd/include/generalregisters.h"
#include "kd/include/psw.h"

// This class implements the general register set for the KDF11_A. This
// processor has R0-R5, two stack pointers (R6) and R7. The stack pointer
// selected is determined by PSW<15:14>. The value 0b00 specifies the
// Kernel stack pointer, the vale 0b11 specifies the user stack pointer.
// (Values 0b01 and 0b10 are either reserved or illegal). Stack pointer
// selection occurs whether the MMU is enabled or not (SR0 bit 0 is a 1).
// See KDF11-A User Guide par. 8.3.3.1.
//
// The KDF11-A User Guide (EK-KDF11-UG-PR2) states in par. 1.4.1 that the 
// KDF11-A has just one stack pointer, but this must be a mistake as par. 
// 3.5.2 describes the two stack pointers and the KDF11-B User Guide does
// document the two stack pointers in par. 1.4.1.
// 
// Par. 8.3.3.1 states "Each mode selects its own corresponding stack pointer.
// Thus, all program references to register R6 use a different register as
// specified by PS<15:14>." That paragraph also states that accessing the
// the stack pointer in Reserved or Illegal memory management mode does not
// cause a halt. We therefore presume the presence of four stack pointers,
// one for each memory management mode.
//
class KDF11_ARegisters : public GeneralRegisters
{
public:
    KDF11_ARegisters (u16 const &psw);
    u16& operator[] (u16 registerNr) override;
    u16& prevModeContents (u16 registerNr) override;
    void writePrevMode (u16 registerNr, u16 contents) override;
    operator registerArray() override;

private:
    enum {numRegisters = 8};

    u16 const &psw_;

    // The registers are defined by the registers_ array, of wich
    // registers_[6] isn't used. The stack pointers are defined in its own
    // array, indexed by the memory management mode.
    u16 registers_[numRegisters];
    u16 R6_[4];

    constexpr u16 memMgmtMode (u16 psw);
    constexpr u16 previousMemMgmtMode (u16 psw);
};

constexpr u16 KDF11_ARegisters::memMgmtMode (u16 psw)
{
    return (psw & PSW_MEM_MGMT_MODE) >> PSW_MEM_MGMT_MODE_P;
}

constexpr u16 KDF11_ARegisters::previousMemMgmtMode (u16 psw)
{
    return (psw & PSW_PREV_MEM_MGMT_MODE) >> PSW_PREV_MEM_MGMT_MODE_P;
}

#endif // _KDF11_AREGISTERS_H_