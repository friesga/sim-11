#include "pdp11/pdp11.h"
#include "ms11p/ms11p.h"

#include <gtest/gtest.h>

TEST (MS11PTest, dataCanBeWrittenAndReadBack)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataWritten {0177777};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (0, dataWritten), StatusCode::OK);
    EXPECT_EQ (ms11p.read (0, &dataRead), StatusCode::OK );
    EXPECT_EQ (dataWritten, dataRead);
}

TEST (MS11PTest, capacityIsOneMB)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataWritten {0177777};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (03777776, BusAddress::Width::_22Bit),
        dataWritten), StatusCode::OK);
    EXPECT_EQ (ms11p.read (BusAddress (03777776, BusAddress::Width::_22Bit),
        &dataRead), StatusCode::OK);
    EXPECT_EQ (dataWritten, dataRead);

    EXPECT_TRUE (ms11p.responsible (BusAddress (03777776, BusAddress::Width::_22Bit)));
    EXPECT_FALSE (ms11p.responsible (BusAddress (04000000, BusAddress::Width::_22Bit)));
}

// This test verifies that bit 13 of the CSR can be set and read back. This
// discriminates a MS11-P/M from a MS11-L.
TEST (MS11PTest, identifiedAsMS11P)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 bit13 {0020000};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (0172100), bit13), StatusCode::OK);
    EXPECT_EQ (ms11p.read (BusAddress (0172100), &dataRead), StatusCode::OK);
    EXPECT_TRUE (dataRead & bit13);
}

// Verify correct checkbits are generated for the value 0123456. The expected
// result (040) is based on ZMSPC0 test 1.
TEST (MS11PTest, correctCheckBitsGenerated)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 bit13 {0020000};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (0172100), 0), StatusCode::OK);

    // Write checkbits to memory
    EXPECT_EQ (ms11p.writeWord (BusAddress (0), 0123456), StatusCode::OK);

    // Set Diagnostic check mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (0172100), 4), StatusCode::OK);

    // Read checkbits in CSR
    EXPECT_EQ (ms11p.read (BusAddress (0), &dataRead), StatusCode::OK);
    EXPECT_EQ (dataRead, 0123456);

    // Read checkbits from CSR
    EXPECT_EQ (ms11p.read (BusAddress (0172100), &dataRead), StatusCode::OK);
    EXPECT_EQ ((dataRead >> 5) & 077, 040);
}