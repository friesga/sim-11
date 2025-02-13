#ifndef _STATUSCODES_H_
#define _STATUSCODES_H_

//
// Simulator status codes
//
enum class StatusCode
{
    Success,
    ArgumentError,
    FunctionNotImplemented,
    IOError,
    NonExistingMemory,
    OpenError,
    ReadOnly,
    UnAttached,
    ParityError
};


#endif // !_STATUSCODES_H_
