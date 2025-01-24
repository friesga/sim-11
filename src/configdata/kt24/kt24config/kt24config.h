#ifndef _KT24CONFIG_H_
#define _KT24CONFIG_H_

#include "devicetype.h"
#include "configdata/m9312/m9312config/m9312config.h"

#include <memory>

using std::shared_ptr;
using std::make_shared;

// The KT24 principally comprises two devices the Unibus Map logic
// and a M9312 (compatible) device. The KT24 therefore has all M9312
// options. Apart from the M9312 options the KT24 also has a "Diagnostic
// ROM enabled" option and jumpers to set adressing limits. The
// "Diagnostic ROM enabled" option is implemented via the "diag-rom"
// option. Addressing limits currently aren't implemented.
//
struct KT24Config : public DeviceType<BusType::Unibus>
{
    KT24Config ();

    shared_ptr<M9312Config> m9312ConfigPtr {};
};

// The default constructor creates a usable KT24Config object
inline KT24Config::KT24Config ()
{
    m9312ConfigPtr = make_shared<M9312Config> ();
}


#endif // _KT24CONFIG_H_