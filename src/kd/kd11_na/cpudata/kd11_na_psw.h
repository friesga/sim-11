#ifndef _KD11_NA_PSW_H_
#define _KD11_NA_PSW_H_

#include "kd/include/psw.h"
#include "basicregister/basicregister.h"

#include <array>
#include <stdexcept>

using std::array;
using std::runtime_error;

class KD11_NA_PSW : public PSW, public BasicRegister
{
public:
    using BasicRegister::operator=;

    KD11_NA_PSW (u16 value);
    operator u16 () const override;
    void set (PSW::ProtectionMode protectionMode, u16 value) override;
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

inline KD11_NA_PSW::operator u16 () const
{
    return value_;
}

// Set the Processor Status Word to the given value conform the given
// protection mode. 
inline void KD11_NA_PSW::set (PSW::ProtectionMode protectionMode, u16 value)
{
    value_ = (protectionMode == PSW::ProtectionMode::Trap || 
                protectionMode == PSW::ProtectionMode::RTI ||
                protectionMode == PSW::ProtectionMode::Reset) ?
            value :
            (value_ & TraceBitMask) | (value & ~TraceBitMask);
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
    throw runtime_error ("Memory Management Mode not implemented");
}

inline PSW::Mode KD11_NA_PSW::previousMode () const
{
    throw runtime_error ("Memory Management Mode not implemented");
}

#endif // _KD11_NA_PSW_H_