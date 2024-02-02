#ifndef _KTF11ASR0_H_
#define _KTF11ASR0_H_

#include "types.h"
#include "basicregister/basicregister.h"

// Definition of KTF11-A Status Register 0
//
class SR0 : public BasicRegister
{
public:
    enum class AbortReason
    {
        NonResident = 0100000,
        PageLengthError = 0040000,
        ReadOnlyAccessViolation = 0020000
    };

    static const u16 AccessViolationModeIndex = 5;
    static const u16 AccessViolationModeMask = (u16) (bitField (2) << AccessViolationModeIndex);
    static const u16 PageNumberIndex = 1;
    static const u16 PageNumberMask = (u16) (bitField (3) << PageNumberIndex);
    static const u16 AbortConditionIndex = 13;
    static const u16 AbortConditionMask = (u16) (bitField (3) << AbortConditionIndex);

    SR0 (u16 value);
    bool managementEnabled ();
    bool accessAborted ();
    void setAbortCondition (AbortReason reason, u16 mode, u16 pageNumber);

private:
    static const u16 EnableManagementIndex = 0;
	static const u16 EnableManagementMask = bitValue (EnableManagementIndex);
    static const u16 AbortFlagsIndex = 13;
    static const u16 AbortFlagsMask = (u16) (bitField (3) << AbortFlagsIndex);
};

inline SR0::SR0 (u16 value)
    : 
    BasicRegister {value}
{}

inline bool SR0::managementEnabled ()
{
    return value_ & EnableManagementMask;
}

inline bool SR0::accessAborted ()
{
    return value_ & AbortFlagsMask;
}

// Report the given abort condition in Status Register 0. Bit 0 will be set as
// otherwise the access wouldn't have been aborted.
// 
// After an MMU abort has occurred, the program must clear SR0<15:13> in order
// to resume monitoring memory management status. (EK-KDF11-UG-PR2)
// This implies that an error must only be set when bits <15:13) are clear.
inline void SR0::setAbortCondition (AbortReason reason, u16 mode, u16 pageNumber)
{
    if ((value_ & AbortConditionMask) == 0)
    {
        value_ = static_cast<u16> (reason) |
            ((mode << AccessViolationModeIndex) & AccessViolationModeMask) |
            ((pageNumber << PageNumberIndex) & PageNumberMask) |
            EnableManagementMask;
    }
}

#endif // _KTF11ASR0_H_