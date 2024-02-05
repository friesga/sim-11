#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "types.h"

// This class models a character as transmitted from the console to the
// DLV11-J's receiver and from the DLV11-J transmitter to its receiver by
// means of its simulated loopback connector. A Character objects contains
// the character to be transmitted plus a Break indication. When the
// transmitter enables the Break condition a Break signal, consisting of a
// continuous spacing condition, is sent over the line.
class Character
{
public:
    Character (u8 character);
    Character (u8 character, bool breakCondition);
    operator u8 ();
    bool isBreak ();

private:
    u8 character_;
    bool breakCondition_;
};

inline Character::Character (u8 character)
    :
    character_ {character},
    breakCondition_ {false}
{}

inline Character::Character (u8 character, bool breakCondition)
    :
    character_ {character},
    breakCondition_ {breakCondition}
{}

inline Character::operator u8 ()
{
    if (breakCondition_)
        return 0;
    
    return character_;
}

inline bool Character::isBreak ()
{
    return breakCondition_;
}


#endif // _CHARACTER_H_
