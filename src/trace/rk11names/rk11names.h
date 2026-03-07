#ifndef _RK11NAMES_H_
#define _RK11NAMES_H_

#include "rk/include/rktypes.h"
#include "rk/rk11d/rk11d.h"

//
// Definitions of free functions, used for output of RK11EventRecord and
// RK11FunctionRecord.
//
namespace RK11Names
{
    string getFunctionName (RKTypes::Function function);
    string getStateName (RK11D::State state);
}

#endif // _RK11NAMES_H_