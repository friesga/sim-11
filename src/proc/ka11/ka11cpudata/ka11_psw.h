#ifndef _KA11_PSW_H_
#define _KA11_PSW_H_

#include "proc/kd/include/psw.h"
#include "basicregister/basicregister.h"

#include <array>
#include <stdexcept>

using std::array;
using std::runtime_error;

// This is an implementation of the Processor Status Word for the KA11 CPU.
// The KA11 has no standard Mememory Management and therefore does not
// implement Memory Management Modes.
//
class KA11_PSW : public PSW, public BasicRegister
{
public:
    using BasicRegister::operator=;

    KA11_PSW (u16 value);
    operator u16 () const override;
    void set (PSW::ProtectionMode protectionMode, u16 value) override;
    bool traceBitSet () override;
    u16 priorityLevel () override;
    PSW::Mode currentMode () const override;
    PSW::Mode previousMode () const override;

private:
    static const u16 ConditionCodesMask = (u16)bitField (4);
    static const u16 TraceBitIndex = 4;
    static const u16 TraceBitMask = (1 << TraceBitIndex);
    static const u16 PriorityLevelIndex = 5;
    static const u16 PriorityLevelMask = (u16)(bitField (3) << PriorityLevelIndex);
};

inline KA11_PSW::KA11_PSW (u16 value)
    :
    BasicRegister {value}
{
}

inline KA11_PSW::operator u16 () const
{
    return value_;
}

// Set the Processor Status Word to the given value conform the given
// protection mode. The PSW Trace Bit can be modified by regular instructions,
// see e.g. PDP-11 Handbook 1969 page 43.
inline void KA11_PSW::set (PSW::ProtectionMode protectionMode, u16 value)
{
    value_ = value;
}

// Return the status (set or clear) of the Trace Bit.
inline bool KA11_PSW::traceBitSet ()
{
    return value_ & TraceBitMask;
}

// Return the Priority Level
inline u16 KA11_PSW::priorityLevel ()
{
    return (value_ & PriorityLevelMask) >> PriorityLevelIndex;
}

// Memory Management Modes are not implemented on the KD11-NA.
inline PSW::Mode KA11_PSW::currentMode () const
{
    throw runtime_error ("Memory Management Mode not implemented");
}

inline PSW::Mode KA11_PSW::previousMode () const
{
    throw runtime_error ("Memory Management Mode not implemented");
}

#endif // _KA11_PSW_H_