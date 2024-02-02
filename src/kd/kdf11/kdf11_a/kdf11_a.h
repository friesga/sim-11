#ifndef _KDF11_A_H_
#define _KDF11_A_H_

#include "kd/kdf11/kdf11.h"
#include "kd/kdf11/cpucontrol/kdf11_a_cpucontrol.h"
#include "kd/kdf11/cpudata/kdf11_acpudata.h"
#include "kd/kdf11/odt/kdf11_a_odt.h"
#include "configdata/kdf11_aconfig/kdf11_aconfig.h"
#include "kd/common/controllogic/controllogic.h"

#include <memory>
#include <vector>

using std::unique_ptr;
using std::shared_ptr;
using std::vector;

// The following versions of the KDF11-A exist:
// 1. KDF11-AA processor with memory management unit (MMU),
// 2. KDF11-AB processor with MMU and floating point,
// 3. KDF11-AC processor without MMU or floating point.
// 
// This class implements the KDF11-AA and KDF11-AC versions. The presence of
// the MMU is configurable. The floating point processor currently isn't
// supported.
// 
// The class KDF11_A starts the control logic which on its turn has to run
// the KDF11_A's cpu and start the KD11_FA's ODT.
//
class KDF11_A : public KDF11
{
public:
    KDF11_A (Qbus* bus);
    KDF11_A (Qbus* bus, shared_ptr<KDF11_AConfig> kdf11_aConfig);
};

#endif // !_KDF11_A_H_
