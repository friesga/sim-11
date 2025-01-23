#ifndef _KT24CONFIG_H_
#define _KT24CONFIG_H_

#include "devicetype.h"
#include "configdata/m9312/m9312config/m9312config.h"

#include <memory>

using std::shared_ptr;

struct KT24Config : public DeviceType<BusType::Unibus>
{
    shared_ptr<M9312Config> m9312ConfigPtr {};
};


#endif // _KT24CONFIG_H_