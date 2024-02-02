#ifndef _KDF11_A_PSW_H_
#define _KDF11_A_PSW_H_

#include "kd/include/psw.h"
#include "basicregister/basicregister.h"

#include <array>

using std::array;

class KDF11_PSW : public PSW, public BasicRegister
{
public:
    KDF11_PSW (u16 value);
    operator u16 () override;
    void set (PSW::ProtectionMode protectionMode, u16 value) override;
    bool traceBitSet () override;
    u16 priorityLevel () override;
    void setPriorityLevel (u16 level) override;
    constexpr PSW::Mode currentMode () const override;
    constexpr PSW::Mode previousMode () const override;

private:
    static const u16 ConditionCodesMask = (u16) bitField (4);
    static const u16 TraceBitIndex = 4;
    static const u16 TraceBitMask = (1 << TraceBitIndex);
    static const u16 PriorityLevelIndex = 5;
	static const u16 PriorityLevelMask = (u16) (bitField (3) << PriorityLevelIndex);
    static const u16 PreviousModeIndex = 12;
    static const u16 PreviousModeMask = (u16) (bitField (2) << PreviousModeIndex);
    static const u16 CurrentModeIndex = 14;
    static const u16 CurrentModeMask = (u16) (bitField (2) << CurrentModeIndex);

    // This structure defines the bit masks for the PSW protection modes as
    // defined in EK-KDF11-UG-PR2 Table 8-1.
    struct ProtectionModeMask
    {
        u16 kernel_;
        u16 user_;
    };

    static const ProtectionModeMask protectionMode_[];

    void setPreviousMode (u16 mode);
};

// This array defines the values the PSW will be masked with to set the
// new value. The mask contains the bits not to be changed by the particular
// access. This implements the PSW protection as defined in EK-KDF11-UG-PR2
// Table 8-1. The array is in order of PSW::ProtectionMode.
inline const KDF11_PSW::ProtectionModeMask KDF11_PSW::protectionMode_[] =
{
    {0000000, 0170340},   // RTI
    {0000000, 0000000},   // Trap
    {0000020, 0000020},   // ExplicitAccess
    {0177420, 0177760},   // MTPS
    {0000000, 0000000},   // Reset
};

inline KDF11_PSW::KDF11_PSW (u16 value)
    : 
    BasicRegister {value}
{}

inline KDF11_PSW::operator u16 ()
{
    return value_;
}

// Set the Processor Status Word to the given value conform the given
// protection mode.
inline void KDF11_PSW::set (PSW::ProtectionMode protectionMode, u16 value)
{
    PSW::Mode presentMode = currentMode ();
    u16 mask = (presentMode == PSW::Mode::Kernel) ?
        protectionMode_[static_cast<u16> (protectionMode)].kernel_ :
        protectionMode_[static_cast<u16> (protectionMode)].user_;

    value_ = (value_ & mask) | (value & ~mask);

    if (protectionMode == PSW::ProtectionMode::Trap)
        setPreviousMode (static_cast<u16> (presentMode));
}

// Return the status (set or clear) of the Trace Bit.
inline bool KDF11_PSW::traceBitSet ()
{
    return value_ & TraceBitMask;
}

// Return the Priority Level
inline u16 KDF11_PSW::priorityLevel ()
{
    return (value_ & PriorityLevelMask) >> PriorityLevelIndex;
}

// Set the priority level to the given level. The level is masked with the
// three lowest bits to make sure no level exceeding level 7 can be set.
inline void KDF11_PSW::setPriorityLevel (u16 level)
{
    value_ = (value_ & ~PriorityLevelMask) | ((level & 07) << PriorityLevelIndex);
}

inline constexpr PSW::Mode KDF11_PSW::currentMode () const
{
    return static_cast<Mode> ((value_ & CurrentModeMask) >> CurrentModeIndex);
}

inline constexpr PSW::Mode KDF11_PSW::previousMode () const
{
    return static_cast<Mode> ((value_ & PreviousModeMask) >> PreviousModeIndex);
}


inline void KDF11_PSW::setPreviousMode (u16 mode)
{
    value_ = (value_ & ~PreviousModeMask) | (mode << PreviousModeIndex);
}

#endif // _KDF11_A_PSW_H_