#ifndef _KW11LCONFIG_H_
#define _KW11LCONFIG_H_

#include "devicetype.h"

// The KW11-L is a line time clock for the Unibus. The CSR and vector address
// and bus request level are fixed and the device doesn't have any
// configuration options. The only purpose of this class is to indicate that
// the KW11-L is present in the system configuration.
//
struct KW11LConfig : public DeviceType<BusType::Unibus>
{};

#endif // _KW11LCONFIG_H_