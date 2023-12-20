#ifndef _KDF11_A_PSW_H_
#define _KDF11_A_PSW_H_

#include "basicregister/basicregister.h"

class KDF11_A_PSW : public BasicRegister
{
public:
    // Definition of the Memory Management Modes as used for the Current
    // Mode (bits <15:14>) Previous Mode (bits <13:12>).
    enum class Mode
    {
        Kernel = 0,
        Reserved = 1,
        Illegal = 2,
        User = 3
    };

    KDF11_A_PSW (u16 value);
    void clearConditionCodes (u16 conditionCodes);
    void setConditionCodes (u16 conditionCodes);
    bool traceBitSet ();
    u16 priorityLevel ();
    void setPriorityLevel (u16 level);
    Mode currentMode ();
    Mode previousMode ();

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
};

inline KDF11_A_PSW::KDF11_A_PSW (u16 value)
    : 
    BasicRegister {value}
{}

inline void KDF11_A_PSW::clearConditionCodes (u16 conditionCodes)
{
    value_ &= ~(conditionCodes & ConditionCodesMask);
}

inline void KDF11_A_PSW::setConditionCodes (u16 conditionCodes)
{
    value_ |= (conditionCodes & ConditionCodesMask);
}

// Return the status (set or clear) of the Trace Bit.
inline bool KDF11_A_PSW::traceBitSet ()
{
    return value_ & TraceBitMask;
}

// Return the Priority Level
inline u16 KDF11_A_PSW::priorityLevel ()
{
    return (value_ & PriorityLevelMask) >> PriorityLevelIndex;
}

// Set the priority level to the given level. The level is masked with the
// three lowest bits to make sure no level exceeding level 7 can be set.
inline void KDF11_A_PSW::setPriorityLevel (u16 level)
{
    value_ = (value_ & ~PriorityLevelMask) | ((level & 07) << PriorityLevelIndex);
}

inline KDF11_A_PSW::Mode KDF11_A_PSW::currentMode ()
{
    return static_cast<Mode> ((value_ & CurrentModeMask) >> CurrentModeIndex);
}

inline KDF11_A_PSW::Mode KDF11_A_PSW::previousMode ()
{
    return static_cast<Mode> ((value_ & PreviousModeMask) >> PreviousModeIndex);
}

#endif // _KDF11_A_PSW_H_