#include "pdp11/pdp11.h"
#include "ms11p/ms11p.h"

#include <gtest/gtest.h>

// Definition of the test fixture in which the CSR bits can be defined
class MS11PTest : public ::testing::Test
{
protected:
    // Use our own set of definitions to avoid common cause issues
    static constexpr u16 MS11P_CSR = 0172100;

    // CSR bit definitions
    static constexpr u16  DiagnosticCheck = 1 << 2;
    static constexpr u16  SingleError     = 1 << 4;
    static constexpr u16  ErrorStorage    = 1 << 5;
    static constexpr u16  InhibitMode     = 1 << 13;
    static constexpr u16  EUBAddress      = 1 << 14;

    static constexpr u16  errorStorageContents (u16 csr) 
        { return (csr >> 5) & 0177; }
};

TEST_F (MS11PTest, dataCanBeWrittenAndReadBack)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataWritten {0177777};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (0, dataWritten), StatusCode::OK);
    EXPECT_EQ (ms11p.read (0, &dataRead), StatusCode::OK );
    EXPECT_EQ (dataWritten, dataRead);
}

TEST_F (MS11PTest, capacityIsOneMB)
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
TEST_F (MS11PTest, identifiedAsMS11P)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), InhibitMode), StatusCode::OK);
    EXPECT_EQ (ms11p.read (BusAddress (MS11P_CSR), &dataRead), StatusCode::OK);
    EXPECT_TRUE (dataRead & InhibitMode);
}

// Verify correct checkbits are generated for the value 0123456. The expected
// result (040) is based on ZMSPC0 test 1.
TEST_F (MS11PTest, correctCheckBitsGenerated)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::OK);

    // Write checkbits to memory
    EXPECT_EQ (ms11p.writeWord (BusAddress (0), 0123456), StatusCode::OK);

    // Set Diagnostic check mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), DiagnosticCheck), StatusCode::OK);

    // Read checkbits in CSR
    EXPECT_EQ (ms11p.read (BusAddress (0), &dataRead), StatusCode::OK);
    EXPECT_EQ (dataRead, 0123456);

    // Read checkbits from CSR
    EXPECT_EQ (ms11p.read (BusAddress (MS11P_CSR), &dataRead), StatusCode::OK);
    EXPECT_EQ (errorStorageContents (dataRead), 040);
}

TEST_F (MS11PTest, checkBitsCanBeWritten)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::OK);

    // Write checkbits to CSR and set Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), (077 << 5) | DiagnosticCheck),
        StatusCode::OK);

    // Write data to memory with check bits from CSR
    EXPECT_EQ (ms11p.writeWord (BusAddress (0), 0123456), StatusCode::OK);

    // Read data plus checkbits in CSR
    EXPECT_EQ (ms11p.read (BusAddress (0), &dataRead), StatusCode::OK);
    EXPECT_EQ (dataRead, 0123456);

    // Read checkbits from CSR
    EXPECT_EQ (ms11p.read (BusAddress (MS11P_CSR), &dataRead), StatusCode::OK);
    EXPECT_EQ (errorStorageContents (dataRead), 077);
}

TEST_F (MS11PTest, singleErrorReported)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::OK);

    // Write checkbits to CSR and set Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR),
        (077 << 5) | DiagnosticCheck), StatusCode::OK);

    // Write data to memory with incorrect check bits from CSR using an addres
    // with some of the seven highest bits set so the contents of the CSR
    // Address Error and Syndrome Storage can be checked.
    EXPECT_EQ (ms11p.writeWord (BusAddress (0174000), 0123456), StatusCode::OK);

    // Reset Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::OK);

    // Read data with incorrect checkbits
    EXPECT_EQ (ms11p.read (BusAddress (0174000), &dataRead),
        StatusCode::OK);
    EXPECT_EQ (dataRead, 0123456);
    
    // Verify Single Error bit set and Address Error and Syndrome Storage
    // contains bits A17-A11 of the address.
    EXPECT_EQ (ms11p.read (BusAddress (MS11P_CSR), &dataRead), StatusCode::OK);
    EXPECT_TRUE (dataRead & SingleError);
    EXPECT_EQ (errorStorageContents (dataRead), 037);
}


TEST_F (MS11PTest, extractErrorLog)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::OK);

    // Write checkbits to CSR and set Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR),
        (077 << 5) | DiagnosticCheck), StatusCode::OK);

    // Write data to memory with incorrect check bits from CSR using the
    // highest available addres.
    EXPECT_EQ (ms11p.writeWord (BusAddress (03777776, BusAddress::Width::_22Bit), 0123456),
        StatusCode::OK);

    // Reset Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::OK);

    // Read data with incorrect checkbits
    EXPECT_EQ (ms11p.read (BusAddress (03777776, BusAddress::Width::_22Bit), &dataRead),
        StatusCode::OK);
    EXPECT_EQ (dataRead, 0123456);

    // Verify Single Error bit set and Address Error and Syndrome Storage
    // contains bits A17-A11 of the address.
    EXPECT_EQ (ms11p.read (BusAddress (MS11P_CSR), &dataRead), StatusCode::OK);
    EXPECT_TRUE (dataRead & SingleError);
    EXPECT_EQ (errorStorageContents (dataRead), 0177);

    // Read high order address bits from CSR
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), EUBAddress),
        StatusCode::OK);
    EXPECT_EQ (ms11p.read (BusAddress (MS11P_CSR), &dataRead), StatusCode::OK);
    EXPECT_EQ (errorStorageContents (dataRead), 03);

    // And finally read the syndrome bits. Stored checkbits (077) xor xorrect
    // checkbits (040) is 037.
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), EUBAddress | DiagnosticCheck),
        StatusCode::OK);
    EXPECT_EQ (ms11p.read (BusAddress (MS11P_CSR), &dataRead), StatusCode::OK);
    EXPECT_EQ (errorStorageContents (dataRead), 037);
}