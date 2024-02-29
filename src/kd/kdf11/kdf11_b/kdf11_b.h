#ifndef _KDF11_B_H_
#define _KDF11_B_H_

#include "kd/kdf11/kdf11.h"
#include "kd/kdf11/cpucontrol/kdf11_cpucontrol.h"
#include "kd/kdf11/odt/kdf11_odt.h"
#include "configdata/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "kd/common/controllogic/controllogic.h"
#include "kd/kdf11/cpudata/kdf11cpudata.h"
#include "kd/kdf11/kdf11_b/serial_line_units/serial_line_units.h"
#include "bdv11/bdv11.h"

#include <memory>
#include <vector>

using std::unique_ptr;
using std::shared_ptr;
using std::vector;

// The KDF11-BA is a quad-height PDP-11 CPU module (M8189). This module
// contains a central processor, memory management unit (MMU), a line
// frequency clock, a BDV11-compatible bootstrap and diagnostic ROM, and two
// serial-line units. Three extra 40-pin sockets are provided for optional
// floating point and commercial instruction sets. The central processor and
// memory management units are functionally compatible with the KDF11-AA CPU
// and MMU.
//
// The KDF11-BA CPU supports up to 256K bytes of memory on a traditional
// LSI-11 bus backplane (18 address bits) or up to 4 megabytes of memory when
// the module is installed in an extended LSI-11 bus backplane (H9276 or
// H9275). The extended LSI-11 bus backplane adds four address lines to the
// LS11-bus to provide a 22-bit addressing capability when the KDF11-BA is
// used with the MSV11-P (M8067) memory module.
//
// The central processor uses the LSI-11 bus or extended LSI-11 bus with
// 4-level interrupt bus protocol. The KDF11-BA is compatible with existing
// LSI-11 processors and peripheral devices. (EK-KDFEB-UG-001)
//
class KDF11_B : public KDF11
{
public:
    KDF11_B (Qbus* bus, shared_ptr<KDF11_BConfig> kdf11_bConfig);

private:
    unique_ptr<SerialLineUnits> serialLineUnits;
    unique_ptr<BDV11> bdv11;
};

#endif // !_KDF11_B_H_
