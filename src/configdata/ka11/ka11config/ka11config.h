#ifndef _KA11CONFIG_H_
#define _KA11CONFIG_H_

#include "devicetype.h"
#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"

#include <optional>

using std::optional;

struct KA11Config : public DeviceType<BusType::Unibus>
{
    optional<KY11_AConfig> ky11_aConfig_ {};
};

#endif _KA11CONFIG_H_