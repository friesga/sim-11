#ifndef _KD11_NA_PSW_H_
#define _KD11_NA_PSW_H_

#include "kd/include/psw.h"
#include "basicregister/basicregister.h"

#include <array>

using std::array;

class KD11_NA_PSW : public PSW, public BasicRegister
{
public:
    KD11_NA_PSW (u16 value);
    operator u16 () override;
    void set (PSW::ProtectionMode protectionMode, u16 value) override;
    void load (u16 value) override;
    bool traceBitSet () override;
    u16 priorityLevel () override;
    void setPriorityLevel (u16 level) override;
    PSW::Mode currentMode () const override;
    PSW::Mode previousMode () const override;

private:
    static const u16 ConditionCodesMask = (u16) bitField (4);
    static const u16 TraceBitIndex = 4;
    static const u16 TraceBitMask = (1 << TraceBitIndex);
    static const u16 PriorityLevelIndex = 5;
	static const u16 PriorityLevelMask = (u16) (bitField (3) << PriorityLevelIndex);
};

inline KD11_NA_PSW::KD11_NA_PSW (u16 value)
    : 
    BasicRegister {value}
{}

inline KD11_NA_PSW::operator u16 ()
{
    return value_;
}

// Set the Processor Status Word to the given value conform the given
// protection mode. 
inline void KD11_NA_PSW::set (PSW::ProtectionMode protectionMode, u16 value)
{
    value_ = (value_ & TraceBitMask) | (value & ~TraceBitMask);
}

inline void KD11_NA_PSW::load (u16 value)
{
    value_ = value;
}

// Return the status (set or clear) of the Trace Bit.
inline bool KD11_NA_PSW::traceBitSet ()
{
    return value_ & TraceBitMask;
}

// Return the Priority Level
inline u16 KD11_NA_PSW::priorityLevel ()
{
    return (value_ & PriorityLevelMask) >> PriorityLevelIndex;
}

// Set the priority level to the given level. The level is masked with the
// three lowest bits to make sure no level exceeding level 7 can be set.
inline void KD11_NA_PSW::setPriorityLevel (u16 level)
{
    value_ = (value_ & ~PriorityLevelMask) | ((level & 07) << PriorityLevelIndex);
}

// Memory Management Modes are not implemented on the KD11-NA.
inline PSW::Mode KD11_NA_PSW::currentMode () const
{
    throw "Memory Management Mode not implemented";
}

inline PSW::Mode KD11_NA_PSW::previousMode () const
{
    throw "Memory Management Mode not implemented";
}

#endif // _KD11_NA_PSW_H_