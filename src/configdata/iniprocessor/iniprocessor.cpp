#include "iniprocessor.h"
#include "../sectionprocessor/sectionprocessor.h"
#include "../ba11nprocessor/ba11nprocessor.h"
#include "configdata/rl/rlprocessor/rlprocessor.h"
#include "configdata/rxv21/rxv21processor/rxv21processor.h"
#include "../bdv11processor/bdv11processor.h"
#include "configdata/serialconfig/dlv11processor/dlv11processor.h"
#include "../msv11processor/msv11processor.h"
#include "../kd11_naprocessor/kd11_naprocessor.h"
#include "../kdf11_aprocessor/kdf11_aprocessor.h"
#include "../kdf11_b/kdf11_bprocessor/kdf11_bprocessor.h"
#include "../ms11pprocessor/ms11pprocessor.h"

using std::make_unique;

unique_ptr<DeviceConfigProcessor> IniProcessor::createBA11_NProcessor ()
{
    return make_unique<BA11_NProcessor> ();
}

// Factory for the creation of RLProcessor objects
unique_ptr<DeviceConfigProcessor> IniProcessor::createRLProcessor ()
{
    return make_unique<RLProcessor> ();
}

// Factory for the creation of RXProcessor objects
unique_ptr<DeviceConfigProcessor> IniProcessor::createRXProcessor ()
{
    return make_unique<RXV21Processor> ();
}

// Factory for the creation of BDV11Processor objects
unique_ptr<DeviceConfigProcessor> IniProcessor::createBDV11Processor ()
{
    return make_unique<BDV11Processor> ();
}


unique_ptr<DeviceConfigProcessor> IniProcessor::createDLV11Processor ()
{
    return make_unique<DLV11Processor> ();
}

unique_ptr<DeviceConfigProcessor> IniProcessor::createMSV11Processor ()
{
    return make_unique<MSV11Processor> ();
}

unique_ptr<DeviceConfigProcessor> IniProcessor::createKD11_NAProcessor ()
{
    return make_unique<KD11_NAProcessor> ();
}

unique_ptr<DeviceConfigProcessor> IniProcessor::createKDF11_AProcessor ()
{
    return make_unique<KDF11_AProcessor> ();
}

unique_ptr<DeviceConfigProcessor> IniProcessor::createKDF11_BProcessor ()
{
    return make_unique<KDF11_BProcessor> ();
}

unique_ptr<DeviceConfigProcessor> IniProcessor::createMS11_PProcessor ()
{
    return make_unique<MS11PProcessor> ();
}
