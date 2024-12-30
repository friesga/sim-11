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