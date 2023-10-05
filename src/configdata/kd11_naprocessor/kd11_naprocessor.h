#ifndef _KD11_NAPROCESSOR_H_
#define _KD11_NAPROCESSOR_H_

#include "../kd11processor/kd11processor.h"
#include "../kd11_naconfig/kd11_naconfig.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class KD11_NAProcessor : public KD11Processor
{
public:
	KD11_NAProcessor ();
	unique_ptr<DeviceConfig> getConfig () override;

private:
	unique_ptr<KD11_NAConfig> kd11ConfigPtr {make_unique<KD11_NAConfig> ()};
};

#endif // _KD11_NAPROCESSOR_H_