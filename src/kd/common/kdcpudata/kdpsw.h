#ifndef _KD_PSW_H_
#define _KD_PSW_H_

#include "kd/include/psw.h"
#include "basicregister/basicregister.h"

class KD_PSW : public PSW, public BasicRegister
{
public:
    KD_PSW (u16 value);
    void clearConditionCodes (u16 conditionCodes) override;
    void setConditionCodes (u16 conditionCodes) override;
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
    static const u16 PreviousModeIndex = 12;
    static const u16 PreviousModeMask = (u16) (bitField (2) << PreviousModeIndex);
    static const u16 CurrentModeIndex = 14;
    static const u16 CurrentModeMask = (u16) (bitField (2) << CurrentModeIndex);
};

inline KD_PSW::KD_PSW (u16 value)
    : 
    BasicRegister {value}
{}

inline void KD_PSW::clearConditionCodes (u16 conditionCodes)
{
    value_ &= ~(conditionCodes & ConditionCodesMask);
}

inline void KD_PSW::setConditionCodes (u16 conditionCodes)
{
    value_ |= (conditionCodes & ConditionCodesMask);
}

// Return the status (set or clear) of the Trace Bit.
inline bool KD_PSW::traceBitSet ()
{
    return value_ & TraceBitMask;
}

// Return the Priority Level
inline u16 KD_PSW::priorityLevel ()
{
    return (value_ & PriorityLevelMask) >> PriorityLevelIndex;
}

// Set the priority level to the given level. The level is masked with the
// three lowest bits to make sure no level exceeding level 7 can be set.
inline void KD_PSW::setPriorityLevel (u16 level)
{
    value_ = (value_ & ~PriorityLevelMask) | ((level & 07) << PriorityLevelIndex);
}

inline PSW::Mode KD_PSW::currentMode () const
{
    return static_cast<Mode> ((value_ & CurrentModeMask) >> CurrentModeIndex);
}

inline PSW::Mode KD_PSW::previousMode () const
{
    return static_cast<Mode> ((value_ & PreviousModeMask) >> PreviousModeIndex);
}

#endif // _KD_PSW_H_