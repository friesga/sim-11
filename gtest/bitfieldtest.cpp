#include "bitfield.h"
#include "types.h"

#include <gtest/gtest.h>

TEST (BitFieldTest, fieldsCanBeSet)
{
    union Register
    {
        uint16_t value;
        BitField<u16, 0, 2> field1; // field1 is bits [0-1]
        BitField<u16, 2, 3> field2; // field2 is bits [2-4]
        BitField<u16, 5, 3> field3; // field3 is bits [5-7]
        BitField<u16, 8, 8> field4; // field4 is bits [8-15]
    };

    Register r {0};
    r.field1 = 1;
    r.field2 = 7;
    r.field4 = 0377;

    EXPECT_EQ (r.value, 0177435);
}

TEST (BitFieldTest, bitsCanBeSet)
{
    union Register
    {
        uint16_t value;
        BitField<u16, 0, 1> bit0;
        BitField<u16, 1, 1> bit1;
        BitField<u16, 2, 1> bit2;
        BitField<u16, 3, 1> bit3;
        BitField<u16, 4, 1> bit4;
        BitField<u16, 5, 1> bit5;
        BitField<u16, 6, 1> bit6;
        BitField<u16, 7, 1> bit7;
        BitField<u16, 8, 1> bit8;
        BitField<u16, 9, 1> bit9;
        BitField<u16, 10, 1> bit10;
        BitField<u16, 11, 1> bit11;
        BitField<u16, 12, 1> bit12;
        BitField<u16, 13, 1> bit13;
        BitField<u16, 14, 1> bit14;
        BitField<u16, 15, 1> bit15;
    };

    Register r {0};
    r.bit0 = 1;
    r.bit1 = 1;
    r.bit2 = 1;
    r.bit3 = 1;
    r.bit4 = 1;
    r.bit5 = 1;
    r.bit6 = 1;
    r.bit7 = 1;
    r.bit8 = 1;
    r.bit9 = 1;
    r.bit10 = 1;
    r.bit11 = 1;
    r.bit12 = 1;
    r.bit13 = 1;
    r.bit14 = 1;
    r.bit15 = 1;

    EXPECT_EQ (r.value, 0177777);
}

TEST (BitFieldTest, correctBitsAreSet)
{
    union Register
    {
        uint16_t value;
        BitField<u16, 0, 1> bit0;
        BitField<u16, 1, 1> bit1;
        BitField<u16, 2, 1> bit2;
        BitField<u16, 3, 1> bit3;
        BitField<u16, 4, 1> bit4;
        BitField<u16, 5, 1> bit5;
        BitField<u16, 6, 1> bit6;
        BitField<u16, 7, 1> bit7;
        BitField<u16, 8, 1> bit8;
        BitField<u16, 9, 1> bit9;
        BitField<u16, 10, 1> bit10;
        BitField<u16, 11, 1> bit11;
        BitField<u16, 12, 1> bit12;
        BitField<u16, 13, 1> bit13;
        BitField<u16, 14, 1> bit14;
        BitField<u16, 15, 1> bit15;
    };

    Register r {0};
    r.bit0 = 1;
    r.bit1 = 0;
    r.bit2 = 1;
    r.bit3 = 0;
    r.bit4 = 1;
    r.bit5 = 0;
    r.bit6 = 1;
    r.bit7 = 0;
    r.bit8 = 1;
    r.bit9 = 0;
    r.bit10 = 1;
    r.bit11 = 0;
    r.bit12 = 1;
    r.bit13 = 0;
    r.bit14 = 1;
    r.bit15 = 0;

    EXPECT_EQ (r.value, 0052525);
}

TEST (BitFieldTest, combinationOfFielsAndBits)
{
    union Register
    {
        uint16_t value;
        BitField<u16, 0, 1> bit0;    // bit 0
        BitField<u16, 1, 1> bit1;    // bit 1
        BitField<u16, 2, 1> bit2;    // bit 2
        BitField<u16, 3, 3> field1;  // field 1 is bits [3-5]
        BitField<u16, 4, 1> bit6;    // bit 6
        BitField<u16, 5, 1> bit7;    // bit 7
        BitField<u16, 6, 1> bit8;    // bit 8
        BitField<u16, 9, 3> field2;  // field 2 is bits [9-11]
        BitField<u16, 10, 1> bit12;  // bit 12
        BitField<u16, 13, 1> bit13;  // bit 13
        BitField<u16, 14, 1> bit14;  // bit 14
        BitField<u16, 15, 1> bit15;  // bit 15
    };

    Register r {0};
    r.field1 = 07;
    r.field2 = 07;
    r.bit0 = 1;
    r.bit15 = 1;

    EXPECT_EQ (r.value, 0107071);
}


TEST (BitFieldTest, valueCanBeSet)
{
    union Register
    {
        uint16_t value;
        BitField<u16, 0, 1> bit0;    // bit 0
        BitField<u16, 1, 1> bit1;    // bit 1
        BitField<u16, 2, 1> bit2;    // bit 2
        BitField<u16, 3, 3> field1;  // field 1 is bits [3-5]
        BitField<u16, 4, 1> bit6;    // bit 6
        BitField<u16, 5, 1> bit7;    // bit 7
        BitField<u16, 6, 1> bit8;    // bit 8
        BitField<u16, 9, 3> field2;  // field 2 is bits [9-11]
        BitField<u16, 10, 1> bit12;  // bit 12
        BitField<u16, 13, 1> bit13;  // bit 13
        BitField<u16, 14, 1> bit14;  // bit 14
        BitField<u16, 15, 1> bit15;  // bit 15
    };

    Register r {0107071};
    EXPECT_EQ (r.field1, 07);
    EXPECT_EQ (r.field2, 07);
    EXPECT_TRUE (r.bit0);
    EXPECT_TRUE (r.bit15);
}