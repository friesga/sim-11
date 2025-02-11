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
    OpenError,
    ReadOnly,
    UnAttached
};


#endif // !_STATUSCODES_H_
