#include "pdp11/pdp11.h"
#include "m9312/m9312.h"
#include "configdata/m9312/m9312config/m9312config.h"

#include <gtest/gtest.h>
#include <memory>

using std::shared_ptr;
using std::make_shared;

TEST (M9312Test, m93isResponsibleForCorrectAddresses)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        true
    };
    
    shared_ptr<M9312Config> m9312ConfigPtr =
        make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    EXPECT_TRUE (m9312.responsible (0165000));
    EXPECT_TRUE (m9312.responsible (0165776));
    EXPECT_TRUE (m9312.responsible (0173000));
    EXPECT_TRUE (m9312.responsible (0173776));

    EXPECT_FALSE (m9312.responsible (0166000));
    EXPECT_FALSE (m9312.responsible (0174000));
}

TEST (M9312Test, diagROMreadCorrectly)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        true
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    CondData<u16> data = m9312.read (0165000);
    EXPECT_EQ (data, 0xEA00);
    EXPECT_EQ (data.statusCode (), StatusCode::Success);
    data = m9312.read (0165002);
    EXPECT_EQ (data, 0xEA00);
    EXPECT_EQ (data.statusCode (), StatusCode::Success);
}

TEST (M9312Test, readOfEmptySocketReturnsError)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::NONE,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        true
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    CondData<u16> data = m9312.read (0165000);
    EXPECT_EQ (data.statusCode (), StatusCode::NonExistingMemory);
}

TEST (M9312Test, bootROMsreadCorrectly)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        true
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    CondData<u16> data = m9312.read (0173000);
    EXPECT_EQ (data, 0x444C);
    EXPECT_EQ (data.statusCode (), StatusCode::Success);

    data = m9312.read (0173002);
    EXPECT_EQ (data, 0x007E);
    EXPECT_EQ (data.statusCode (), StatusCode::Success);

    data = m9312.read (0173004);
    EXPECT_EQ (data, 0x00B1);
    EXPECT_EQ (data.statusCode (), StatusCode::Success);

    data = m9312.read (0173200);
    EXPECT_EQ (data, 0x444D);
    EXPECT_EQ (data.statusCode (), StatusCode::Success);

    data = m9312.read (0173400);
    EXPECT_EQ (data, 0x4458);
    EXPECT_EQ (data.statusCode (), StatusCode::Success);

    data = m9312.read (0173600);
    EXPECT_EQ (data, 0x4450);
    EXPECT_EQ (data.statusCode (), StatusCode::Success);
}

TEST (M9312Test, romCannotBeWritten)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        true
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    u16 data {0};
    EXPECT_EQ (m9312.writeWord (0165000, data), StatusCode::NonExistingMemory);
}


TEST (M9312Test, addressOffsetSwitchBankCanBeRead)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0165000,
        true
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    CondData<u16> data = m9312.read (0173024);
    EXPECT_EQ (data, 0165000);
    EXPECT_EQ (data.statusCode (), StatusCode::Success);
}