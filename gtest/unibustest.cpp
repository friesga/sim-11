#include "bus/unibus/unibus.h"
#include "ms11p/ms11p.h"
#include "kt24/kt24.h"

#include <gtest/gtest.h>

TEST (UnibusTest, _16bitAddresCanBeRead)
{
    Unibus bus;
    MS11P ms11p (&bus);
    u16 dataWritten {0177777};
    CondData<u16> dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);

    // KT24 is disabled by default

    EXPECT_EQ (ms11p.writeWord (0157776, dataWritten), StatusCode::OK);
    dataRead = bus.read (0157776);

    EXPECT_TRUE (dataRead.hasValue ());
    EXPECT_EQ (dataWritten, dataRead);
}

TEST (UnibusTest, _18bitAddresReadIsMapped)
{
    Unibus bus;
    MS11P ms11p (&bus);
    KT24 kt24 (&bus);

    u16 dataWritten {0177777};
    CondData<u16> dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);
    bus.installModule (&kt24);

    // Set up mapping registers. KT24 is disabled by default.
    kt24.enable ();
    kt24.writeWord (0170370, 010000);
    kt24.writeWord (0170372, 0);

    // 074000 -> 010000
    EXPECT_EQ (ms11p.writeWord (010000, dataWritten), StatusCode::OK);
    dataRead = bus.read (BusAddress (0740000, BusAddress::Width::_18Bit));

    EXPECT_TRUE (dataRead.hasValue ());
    EXPECT_EQ (dataWritten, dataRead);
}

TEST (UnibusTest, _22bitAddresWithHighestBitSetReadIsMapped)
{
    Unibus bus;
    MS11P ms11p (&bus);
    KT24 kt24 (&bus);

    u16 dataWritten {0177777};
    CondData<u16> dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);
    bus.installModule (&kt24);

    // Set up mapping registers. KT24 is disabled by default.
    kt24.enable ();
    kt24.writeWord (0170370, 010000);
    kt24.writeWord (0170372, 0);

    // 01774000 -> 010000
    EXPECT_EQ (ms11p.writeWord (010000, dataWritten), StatusCode::OK);
    dataRead = bus.read (BusAddress (017740000, BusAddress::Width::_22Bit));

    EXPECT_TRUE (dataRead.hasValue ());
    EXPECT_EQ (dataWritten, dataRead);
}

TEST (UnibusTest, _18bitAddresWriteWordIsMapped)
{
    Unibus bus;
    MS11P ms11p (&bus);
    KT24 kt24 (&bus);

    u16 dataWritten {0177777};
    u16 dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);
    bus.installModule (&kt24);

    // Set up mapping registers. KT24 is disabled by default.
    kt24.enable ();
    kt24.writeWord (0170370, 010000);
    kt24.writeWord (0170372, 0);

    // 074000 -> 010000
    EXPECT_TRUE (bus.writeWord (BusAddress (0740000, BusAddress::Width::_18Bit), dataWritten));
    EXPECT_EQ (ms11p.read (010000, &dataRead), StatusCode::OK);
    EXPECT_EQ (dataWritten, dataRead);
}

TEST (UnibusTest, _22bitAddresWithHighestBitSetWriteWordIsMapped)
{
    Unibus bus;
    MS11P ms11p (&bus);
    KT24 kt24 (&bus);

    u16 dataWritten {0177777};
    u16 dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);
    bus.installModule (&kt24);

    // Set up mapping registers. KT24 is disabled by default.
    kt24.enable ();
    kt24.writeWord (0170370, 010000);
    kt24.writeWord (0170372, 0);

    // 01774000 -> 010000
    EXPECT_TRUE (bus.writeWord (BusAddress (017740000, BusAddress::Width::_18Bit), dataWritten));
    EXPECT_EQ (ms11p.read (010000, &dataRead), StatusCode::OK);
    EXPECT_EQ (dataWritten, dataRead);
}