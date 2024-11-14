#ifndef _STATUSCODES_H_
#define _STATUSCODES_H_

//
// Simulator status codes
//
enum class StatusCode
{
    OK,
    ArgumentError,
    FunctionNotImplemented,
    IOError,
    NonExistingMemory,
    NotAttachable,
    OpenError,
    ReadOnly,
    ReadOnlyNotAllowed,
    UnAttached
};


#endif // !_STATUSCODES_H_
