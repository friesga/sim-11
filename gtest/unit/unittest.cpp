#include "unit/unit.h"

#include <gtest/gtest.h>
#include <array>

using std::array;

TEST (UnitTest, fileCanBeCreated)
{
    Unit unit {};
    EXPECT_EQ (unit.attachUnit ("test.dsk",
        Bitmask<AttachFlags> {AttachFlags::NewFile | AttachFlags::Overwrite}),
        StatusCode::Success);

    EXPECT_TRUE (unit.isAttached ());
}

TEST (UnitTest, fileCanBeWrittenAndReadBack)
{
    Unit unit {};
    EXPECT_EQ (unit.attachUnit ("test.dsk",
        Bitmask<AttachFlags> {AttachFlags::NewFile | AttachFlags::Overwrite}),
        StatusCode::Success);

    array<uint16_t, 512> buffer {};
    buffer.fill (0177777);

    EXPECT_EQ (unit.writeDataToSector (0, buffer.data (), buffer.size ()),
        buffer.size ());

    // Verify the written data can be read back
    buffer.fill (0);
    EXPECT_EQ (unit.readDataFromSector (0, buffer.data (), buffer.size ()),
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
    EXPECT_EQ (unit.attachUnit ("test.dsk",
        Bitmask<AttachFlags> {AttachFlags::NewFile | AttachFlags::Overwrite}),
        StatusCode::Success);

    static constexpr int32_t wordsPerSector = 128;
    static constexpr int32_t sectorsPerSurface = 40;
    static constexpr int32_t surfacesPerCylinder = 2;
    static constexpr int32_t cylindersPerCartridge = 256;
    static constexpr int32_t RL01_WordsPerCartridge =
        cylindersPerCartridge * surfacesPerCylinder * sectorsPerSurface *
        wordsPerSector;

    EXPECT_EQ (unit.createBadBlockTable (sectorsPerSurface, wordsPerSector,
        RL01_WordsPerCartridge), StatusCode::Success);

    int32_t Offset1stSectorOfLastTrack =
        (cylindersPerCartridge * surfacesPerCylinder - 1) *
            sectorsPerSurface * wordsPerSector * sizeof (u16);

    array<uint16_t, 512> buffer {};
    EXPECT_EQ (unit.readDataFromSector (Offset1stSectorOfLastTrack,
        buffer.data (), buffer.size ()), buffer.size ());

    EXPECT_EQ (buffer[0], 067000);
}