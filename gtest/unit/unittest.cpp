#include "unit/unit.h"
#include "unit/geometry/geometry.h"

#include <gtest/gtest.h>
#include <array>

using std::array;

TEST (UnitTest, fileCanBeCreated)
{
    Unit unit {};
    EXPECT_EQ (unit.attachUnit ("test.dsk", Geometry {},
        Bitmask<AttachFlags> {AttachFlags::NewFile | AttachFlags::Overwrite}),
        StatusCode::Success);

    EXPECT_TRUE (unit.isAttached ());
}

TEST (UnitTest, fileCanBeWrittenAndReadBack)
{
    Unit unit {};
    Geometry geometry {40, 2, 256, 128};
    DiskAddress diskAdress {0, 0, 0};

    EXPECT_EQ (unit.attachUnit ("test.dsk", geometry,
        Bitmask<AttachFlags> {AttachFlags::NewFile | AttachFlags::Overwrite}),
        StatusCode::Success);

    array<uint16_t, 512> buffer {};
    buffer.fill (0177777);

    EXPECT_EQ (unit.writeDataToSector (diskAdress, buffer.data (), buffer.size ()),
        buffer.size ());

    // Verify the written data can be read back
    buffer.fill (0);
    EXPECT_EQ (unit.readDataFromSector (diskAdress, buffer.data (), buffer.size ()),
        buffer.size ());

    for (int i = 0; i < buffer.size (); ++i)
        EXPECT_EQ (buffer[i], 0177777);
}

// Verify a bad block table is written at the correct location, i.e. on the
// last track of the device. The first word of the first sector of that track
// should contain the value 067000. The bad block table is checked with the
// geometry of a RL01 disk.
TEST (UnitTest, badBlockTableWritten)
{
    Unit unit {};

    static constexpr int32_t wordsPerSector = 128;
    static constexpr int32_t sectorsPerSurface = 40;
    static constexpr int32_t surfacesPerCylinder = 2;
    static constexpr int32_t RL01cylindersPerCartridge = 256;
    static constexpr int32_t RL01_WordsPerCartridge =
        RL01cylindersPerCartridge * surfacesPerCylinder * sectorsPerSurface *
        wordsPerSector;

    Geometry rl01Geometry {sectorsPerSurface, surfacesPerCylinder,
        RL01cylindersPerCartridge, wordsPerSector};

    EXPECT_EQ (unit.attachUnit ("test.dsk", rl01Geometry,
        Bitmask<AttachFlags> {AttachFlags::NewFile | AttachFlags::Overwrite}),
        StatusCode::Success);

    EXPECT_EQ (unit.createBadBlockTable (), StatusCode::Success);

    DiskAddress firstSectorOfLastTrack {0, 1, RL01cylindersPerCartridge - 1};

    array<uint16_t, 128> buffer {};
    EXPECT_EQ (unit.readDataFromSector (firstSectorOfLastTrack,
        buffer.data (), buffer.size ()), buffer.size ());

    EXPECT_EQ (buffer[0], 067000);
}