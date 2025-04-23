#ifndef _RK05_H_
#define _RK05_H_

#include "unit/unit.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"
#include "pdp11peripheral/pdp11peripheral.h"

#include <memory>

using std::shared_ptr;

class RK05 : public Unit
{
public:
    RK05 (Bus* bus, PDP11Peripheral* controller, shared_ptr<RK05Config> configPtr);
};


#endif // _RK05_H_