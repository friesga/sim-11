#ifndef _KDF11_A_H_
#define _KDF11_A_H_

#include "qbus/qbus.h"
#include "configdata/kdf11_aconfig/kdf11_aconfig.h"

#include <memory>

using std::shared_ptr;

class KDF11_A
{
public:
    KDF11_A (Qbus *bus);
    KDF11_A (Qbus *bus, shared_ptr<KDF11_AConfig> kd11_naConfig);

private:
    Qbus *bus_;
};


#endif // _KDF11_A_H_