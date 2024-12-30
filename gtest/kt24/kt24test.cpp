#include "pdp11/pdp11.h"
#include "kt24/kt24.h"
#include "ms11p/ms11p.h"

#include <gtest/gtest.h>
#include <vector>

using std::vector;

// Verify that if the KT24 is disabled data is written to the
// lower 18-bit address space of the given 22-bit address
TEST (KT24, writePassthrough)
{
    Qbus bus;
    KT24 kt24 (&bus);
    MS11P ms11p (&bus);
    u16 dataWritten {0177777};
    u16 dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);

    EXPECT_EQ (kt24.dmaWrite (0, dataWritten), StatusCode::OK);
    EXPECT_EQ (ms11p.read (0, &dataRead), StatusCode::OK);

    EXPECT_EQ (dataWritten, dataRead);
}

// Verify that if the KT24 is disabled data is read from the
// lower 18-bit address space of the given 22-bit address
TEST (KT24, readPassthrough)
{
    Qbus bus;
    KT24 kt24 (&bus);
    MS11P ms11p (&bus);
    u16 dataWritten {0177777};
    u16 dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);

    // KT24 is disabled by default

    EXPECT_EQ (ms11p.writeWord (0, dataWritten), StatusCode::OK);
    EXPECT_EQ (kt24.dmaRead (0, &dataRead), StatusCode::OK);
    
    EXPECT_EQ (dataWritten, dataRead);
}

// Verify the KT24 is responsible for the registers in the range 0170200
// up to and including 0170376.
TEST (KT24, kt24IsResponsible)
{
    Qbus bus;
    KT24 kt24 (&bus);

    EXPECT_FALSE (kt24.responsible (0170176));
    EXPECT_TRUE  (kt24.responsible (0170200));
    EXPECT_TRUE  (kt24.responsible (0170376));
    EXPECT_FALSE (kt24.responsible (0170400));
}

TEST (KT24, kt24RegistersAreIntizalised)
{
    Qbus bus;
    KT24 kt24 (&bus);
    u16 value;

    EXPECT_EQ (kt24.read (0170200, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);
    EXPECT_EQ (kt24.read (0170202, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);
    EXPECT_EQ (kt24.read (0170374, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);
    EXPECT_EQ (kt24.read (0170376, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);
}

TEST (KT24, dmaWriteAddressIsMapped)
{
    struct TestCase
    {
        u16 mapRegisterLowAddress {0};
        u16 mapRegisterLowContents {0};
        u16 mapRegisterHighAddress {0};
        u16 mapRegisterHighContents {0};
        BusAddress _18BitAddress {0};
        u32 physicalAddress {0};
    };

    vector<TestCase> testData =
    {
        {0170200, 0, 0170202, 0, BusAddress (0, BusAddress::Width::_18Bit), 0},
        {0170200, 0200, 0170202, 0, BusAddress (0, BusAddress::Width::_18Bit), 0200},
        {0170200, 0200, 0170202, 0, BusAddress (017776, BusAddress::Width::_18Bit), 020176},
        {0170200, 0200, 0170202, 0, BusAddress (020000, BusAddress::Width::_18Bit), 0},
        {0170200, 0, 0170202, 1, BusAddress (0, BusAddress::Width::_18Bit), 000200000},
        {0170200, 0, 0170202, 077, BusAddress (0, BusAddress::Width::_18Bit), 017600000},
        {0170200, 0, 0170202, 077, BusAddress (017776, BusAddress::Width::_18Bit), 017617776},
        {0170374, 0, 0170376, 0, BusAddress (0740000, BusAddress::Width::_18Bit), 0},
        {0170374, 0, 0170376, 0, BusAddress (0757776, BusAddress::Width::_18Bit), 017776},
    };

    Qbus bus;
    KT24 kt24 (&bus);
    MS11P ms11p (&bus);
    u16 dataWritten {0177777};
    u16 dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);

    kt24.enable ();

    for (TestCase &testCase : testData)
    {
        kt24.writeWord (testCase.mapRegisterLowAddress,
            testCase.mapRegisterLowContents);
        kt24.writeWord (testCase.mapRegisterHighAddress,
            testCase.mapRegisterHighContents);

        EXPECT_EQ (kt24.dmaWrite (testCase._18BitAddress, dataWritten), StatusCode::OK);
        EXPECT_EQ (ms11p.read (testCase.physicalAddress, &dataRead), StatusCode::OK);
        EXPECT_EQ (dataWritten, dataRead);
    }
}

TEST (KT24, dmaReadAddressIsMapped)
{
    struct TestCase
    {
        u16 mapRegisterLowAddress {0};
        u16 mapRegisterLowContents {0};
        u16 mapRegisterHighAddress {0};
        u16 mapRegisterHighContents {0};
        BusAddress _18BitAddress {0};
        u32 physicalAddress {0};
    };

    vector<TestCase> testData =
    {
        {0170200, 0, 0170202, 0, BusAddress (0, BusAddress::Width::_18Bit), 0},
        {0170200, 0200, 0170202, 0, BusAddress (0, BusAddress::Width::_18Bit), 0200},
        {0170200, 0200, 0170202, 0, BusAddress (017776, BusAddress::Width::_18Bit), 020176},
        {0170200, 0200, 0170202, 0, BusAddress (020000, BusAddress::Width::_18Bit), 0},
        {0170200, 0, 0170202, 1, BusAddress (0, BusAddress::Width::_18Bit), 000200000},
        {0170200, 0, 0170202, 077, BusAddress (0, BusAddress::Width::_18Bit), 017600000},
        {0170200, 0, 0170202, 077, BusAddress (017776, BusAddress::Width::_18Bit), 017617776},
        {0170374, 0, 0170376, 0, BusAddress (0740000, BusAddress::Width::_18Bit), 0},
        {0170374, 0, 0170376, 0, BusAddress (0757776, BusAddress::Width::_18Bit), 017776},
    };

    Qbus bus;
    KT24 kt24 (&bus);
    MS11P ms11p (&bus);
    u16 dataWritten {0177777};
    u16 dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);

    kt24.enable ();

    for (TestCase& testCase : testData)
    {
        kt24.writeWord (testCase.mapRegisterLowAddress,
            testCase.mapRegisterLowContents);
        kt24.writeWord (testCase.mapRegisterHighAddress,
            testCase.mapRegisterHighContents);

        EXPECT_EQ (ms11p.writeWord (testCase.physicalAddress, dataWritten), StatusCode::OK);
        EXPECT_EQ (kt24.dmaRead (testCase._18BitAddress, &dataRead), StatusCode::OK);

        EXPECT_EQ (dataRead, dataWritten);
    }
}

// Bit 0 of the low mapping register is ignored as Qbus::writeWord() and
// Qbus::read() prevent writes to and reads from odd addresses.
TEST (KT24, lowMappingRegisterBit0IsIgnored)
{
    Qbus bus;
    KT24 kt24 (&bus);
    MS11P ms11p (&bus);
    u16 dataWritten {0177777};
    u16 dataRead {0};

    // Install devices on the bus
    bus.installModule (&ms11p);

    kt24.enable ();

    kt24.writeWord (0170200, 0);
    kt24.writeWord (0170200, 1);

    EXPECT_EQ (kt24.dmaWrite (0, dataWritten), StatusCode::OK);
    EXPECT_EQ (ms11p.read (0, &dataRead), StatusCode::OK);
    EXPECT_EQ (dataWritten, dataRead);
}
