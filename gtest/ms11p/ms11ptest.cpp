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
    static constexpr u16  DiagnosticCheck    = 1 << 2;
    static constexpr u16  SingleError        = 1 << 4;
    static constexpr u16  ErrorStorage       = 1 << 5;
    static constexpr u16  InhibitMode        = 1 << 13;
    static constexpr u16  EUBAddress         = 1 << 14;
    static constexpr u16  UncorrectableError = 1 << 15;

    static constexpr u16  errorStorageContents (u16 csr) 
        { return (csr >> 5) & 0177; }
};

TEST_F (MS11PTest, dataCanBeWrittenAndReadBack)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataWritten {0177777};
    CondData<u16> dataRead {0};

    EXPECT_EQ (ms11p.writeWord (0, dataWritten), StatusCode::Success);
    dataRead = ms11p.read (0);
    EXPECT_EQ (dataWritten, dataRead);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);
}

TEST_F (MS11PTest, capacityIsOneMB)
{
    Qbus bus;
    MS11P ms11p {&bus};
    u16 dataWritten {0177777};
    CondData<u16> dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (03777776, BusAddress::Width::_22Bit),
        dataWritten), StatusCode::Success);
    dataRead = ms11p.read (BusAddress (03777776, BusAddress::Width::_22Bit));
    EXPECT_EQ (dataWritten, dataRead);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    EXPECT_TRUE (ms11p.responsible (BusAddress (03777776, BusAddress::Width::_22Bit)));
    EXPECT_FALSE (ms11p.responsible (BusAddress (04000000, BusAddress::Width::_22Bit)));
}

// This test verifies that bit 13 of the CSR can be set and read back. This
// discriminates a MS11-P/M from a MS11-L.
TEST_F (MS11PTest, identifiedAsMS11P)
{
    Qbus bus;
    MS11P ms11p {&bus};
    CondData<u16> dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), InhibitMode), StatusCode::Success);
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_TRUE (dataRead & InhibitMode);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);
}

// Verify correct checkbits are generated for the value 0123456. The expected
// result (040) is based on ZMSPC0 test 1.
TEST_F (MS11PTest, correctCheckBitsGenerated)
{
    Qbus bus;
    MS11P ms11p {&bus};
    CondData<u16> dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::Success);

    // Write checkbits to memory
    EXPECT_EQ (ms11p.writeWord (BusAddress (0), 0123456), StatusCode::Success);

    // Set Diagnostic check mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), DiagnosticCheck), StatusCode::Success);

    // Read checkbits in CSR
    dataRead = ms11p.read (BusAddress (0));
    EXPECT_EQ (dataRead, 0123456);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    // Read checkbits from CSR
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_EQ (errorStorageContents (dataRead), 040);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);
}

TEST_F (MS11PTest, checkBitsCanBeWritten)
{
    Qbus bus;
    MS11P ms11p {&bus};
    CondData<u16> dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::Success);

    // Write checkbits to CSR and set Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), (077 << 5) | DiagnosticCheck),
        StatusCode::Success);

    // Write data to memory with check bits from CSR
    EXPECT_EQ (ms11p.writeWord (BusAddress (0), 0123456), StatusCode::Success);

    // Read data plus checkbits in CSR. This generates a double error but no
    // error is logged as the MS11-P is in Diagnostic Mode.
    dataRead = ms11p.read (BusAddress (0));
    EXPECT_EQ (dataRead, 0123456);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    // Read checkbits from CSR
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_EQ (errorStorageContents (dataRead), 077);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);
}

TEST_F (MS11PTest, singleErrorReported)
{
    Qbus bus;
    MS11P ms11p {&bus};
    CondData<u16> dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::Success);

    // Write checkbits that will generate a single bit error to CSR (correct
    // checkbits are 040) and set Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR),
        (041 << 5) | DiagnosticCheck), StatusCode::Success);

    // Write data to memory with incorrect check bits from CSR using an addres
    // with some of the seven highest bits set so the contents of the CSR
    // Address Error and Syndrome Storage can be checked.
    EXPECT_EQ (ms11p.writeWord (BusAddress (0174000), 0123456), StatusCode::Success);

    // Reset Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::Success);

    // Read data with incorrect checkbits
    dataRead = ms11p.read (BusAddress (0174000));
    EXPECT_EQ (dataRead, 0123456);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);
    
    // Verify Single Error bit set and Address Error and Syndrome Storage
    // contains bits A17-A11 of the address.
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_TRUE (dataRead & SingleError);
    EXPECT_EQ (errorStorageContents (dataRead), 037);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);
}


TEST_F (MS11PTest, singleErrorFillsErrorLog)
{
    Qbus bus;
    MS11P ms11p {&bus};
    CondData<u16> dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::Success);

    // Write checkbits that will generate a single bit error to CSR (correct
    // checkbits are 040) and set Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR),
        (041 << 5) | DiagnosticCheck), StatusCode::Success);

    // Write data to memory with incorrect check bits from CSR using the
    // highest available addres.
    EXPECT_EQ (ms11p.writeWord (BusAddress (03777776, BusAddress::Width::_22Bit), 0123456),
        StatusCode::Success);

    // Reset Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::Success);

    // Read data with incorrect checkbits
    dataRead = ms11p.read (BusAddress (03777776, BusAddress::Width::_22Bit));
    EXPECT_EQ (dataRead, 0123456);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    // Verify Single Error bit set and Address Error and Syndrome Storage
    // contains bits A17-A11 of the address.
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_TRUE (dataRead & SingleError);
    EXPECT_EQ (errorStorageContents (dataRead), 0177);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    // Read high order address bits from CSR
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), EUBAddress),
        StatusCode::Success);
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_EQ (errorStorageContents (dataRead), 03);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    // And finally read the syndrome bits. Stored checkbits (041) xor correct
    // checkbits (040) is 001.
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), EUBAddress | DiagnosticCheck),
        StatusCode::Success);
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_EQ (errorStorageContents (dataRead), 001);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);
}

TEST_F (MS11PTest, doubleErrorFillsErrorLog)
{
    Qbus bus;
    MS11P ms11p {&bus};
    CondData<u16> dataRead {0};

    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::Success);

    // Write checkbits that will generate a double bit error to CSR (correct
    // checkbits are 040) and set Diagnostic Check Mode.
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR),
        (043 << 5) | DiagnosticCheck), StatusCode::Success);

    // Write data to memory with incorrect check bits from CSR using the
    // highest available addres.
    EXPECT_EQ (ms11p.writeWord (BusAddress (03777776, BusAddress::Width::_22Bit), 0123456),
        StatusCode::Success);

    // Reset Diagnostic Check Mode
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), 0), StatusCode::Success);

    // Read data with incorrect checkbits
    dataRead = ms11p.read (BusAddress (03777776, BusAddress::Width::_22Bit));
    EXPECT_EQ (dataRead, 0123456);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    // Verify Double Error bit set and Address Error and Syndrome Storage
    // contains bits A17-A11 of the address.
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_TRUE (dataRead & UncorrectableError);
    EXPECT_EQ (errorStorageContents (dataRead), 0177);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    // Read high order address bits from CSR
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), EUBAddress),
        StatusCode::Success);
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_EQ (errorStorageContents (dataRead), 03);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    // And finally read the syndrome bits. Stored checkbits (043) xor correct
    // checkbits (040) is 003.
    EXPECT_EQ (ms11p.writeWord (BusAddress (MS11P_CSR), EUBAddress | DiagnosticCheck),
        StatusCode::Success);
    dataRead = ms11p.read (BusAddress (MS11P_CSR));
    EXPECT_EQ (errorStorageContents (dataRead), 003);
    EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);
}