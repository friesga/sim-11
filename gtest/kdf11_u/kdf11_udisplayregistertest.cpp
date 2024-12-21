#include "pdp11/pdp11.h"
#include "kd/kdf11/kdf11_u/kdf11_u.h"
#include "configdata/m9312/m9312config/m9312config.h"
#include "busaddress/busaddress.h"

#include <gtest/gtest.h>
#include <memory>

using std::shared_ptr;
using std::make_shared;

TEST (KDF11_UDisplayRegistertest, displayRegisterCanBeWritten)
{
    KDF11_UConfig kdf11_uConfig {};

    shared_ptr<KDF11_UConfig> kdf11_uConfigPtr =
        make_shared<KDF11_UConfig> (kdf11_uConfig);

    Qbus bus;
    KDF11_U kdf11_u (&bus, kdf11_uConfigPtr);
    bus.installModule (&kdf11_u);

    // The KDF11-U has to be started to be able to process the exit event
    kdf11_u.start ();

    EXPECT_EQ (bus.writeWord (BusAddress (0177570, BusAddress::Width::_16Bit), 0), true);
}

TEST (KDF11_UDisplayRegistertest, displayRegisterCanNotBeRead)
{
    KDF11_UConfig kdf11_uConfig {};

    shared_ptr<KDF11_UConfig> kdf11_uConfigPtr =
        make_shared<KDF11_UConfig> (kdf11_uConfig);

    Qbus bus;
    KDF11_U kdf11_u (&bus, kdf11_uConfigPtr);
    bus.installModule (&kdf11_u);

    // The KDF11-U has to be started to be able to process the exit event
    kdf11_u.start ();

    EXPECT_FALSE (bus.read (BusAddress (0177570, BusAddress::Width::_16Bit)).hasValue ());
}