#ifndef _KDF11_APROCESSOR_H_
#define _KDF11_APROCESSOR_H_

#include "../kd11processor/kd11processor.h"
#include "../kdf11_aconfig/kdf11_aconfig.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class KDF11_AProcessor : public KD11Processor
{
public:
	KDF11_AProcessor ();
	unique_ptr<DeviceConfig> getConfig () override;

private:
	unique_ptr<KDF11_AConfig> kd11ConfigPtr {make_unique<KDF11_AConfig> ()};
};

#endif // _KDF11_APROCESSOR_H_