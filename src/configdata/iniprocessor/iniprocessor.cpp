#include "iniprocessor.h"
#include "../sectionprocessor/sectionprocessor.h"
#include "../ba11nprocessor/ba11nprocessor.h"
#include "../rlprocessor/rlprocessor.h"
#include "../rxv21processor/rxv21processor.h"
#include "../bdv11processor/bdv11processor.h"
#include "../dlv11processor/dlv11processor.h"
#include "../msv11processor/msv11processor.h"
#include "../kd11_naprocessor/kd11_naprocessor.h"
#include "../kdf11_aprocessor/kdf11_aprocessor.h"

using std::make_unique;

unique_ptr<SectionProcessor> IniProcessor::createBA11_NProcessor ()
{
    return make_unique<BA11_NProcessor> ();
}

// Factory for the creation of RLProcessor objects
unique_ptr<SectionProcessor> IniProcessor::createRLProcessor ()
{
    return make_unique<RLProcessor> ();
}

// Factory for the creation of RXProcessor objects
unique_ptr<SectionProcessor> IniProcessor::createRXProcessor ()
{
    return make_unique<RXV21Processor> ();
}

// Factory for the creation of BDV11Processor objects
unique_ptr<SectionProcessor> IniProcessor::createBDV11Processor ()
{
    return make_unique<BDV11Processor> ();
}


unique_ptr<SectionProcessor> IniProcessor::createDLV11Processor ()
{
    return make_unique<DLV11Processor> ();
}

unique_ptr<SectionProcessor> IniProcessor::createMSV11Processor ()
{
    return make_unique<MSV11Processor> ();
}

unique_ptr<SectionProcessor> IniProcessor::createKD11_NAProcessor ()
{
    return make_unique<KD11_NAProcessor> ();
}

unique_ptr<SectionProcessor> IniProcessor::createKDF11_AProcessor ()
{
    return make_unique<KDF11_AProcessor> ();
}