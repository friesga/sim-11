#include "pdp11/pdp11.h"
#include "bus/unibus/unibus.h"
#include "configdata/m792/m792config/m792config.h"
#include "m792/m792.h"
#include "conddata/conddata.h"

#include <gtest/gtest.h>

TEST (M792Test, romImageCanBeRead)
{
    Unibus bus;
    M792 m792 {&bus, M792Config {}};

    CondData<u16> data = m792.read (BusAddress (0173100));
    ASSERT_TRUE (data.hasValue ());

    EXPECT_EQ (data, 0013701);
    EXPECT_EQ (m792.read (BusAddress (0173102)), 0177570);
}
