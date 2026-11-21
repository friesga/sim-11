#include "basicregister/registerfield.h"

#include <gtest/gtest.h>
#include <type_traits>

// Field definitions used in the tests
using Bit0 = BitField<u16, 0>;
using Bit5 = BitField<u16, 5>;
using Bit15 = BitField<u16, 15>;
using Field4_3 = BitField<u16, 4, 3>;       // Bits 4-6
using Field13_3 = BitField<u16, 13, 3>;     // Bits 13-15
using Field0_4 = BitField<u16, 0, 4>;       // Bits 0-3
using Field0_16 = BitField<u16, 0, 16>;     // The complete register

TEST (RegisterFieldTest, maskIsCorrect)
{
    EXPECT_EQ (RegisterField<Bit0>::mask, 0000001);
    EXPECT_EQ (RegisterField<Bit5>::mask, 0000040);
    EXPECT_EQ (RegisterField<Bit15>::mask, 0100000);
    EXPECT_EQ (RegisterField<Field4_3>::mask, 0000160);
    EXPECT_EQ (RegisterField<Field13_3>::mask, 0160000);
    EXPECT_EQ (RegisterField<Field0_16>::mask, 0177777);
}

TEST (RegisterFieldTest, fieldIsNotCopyable)
{
    EXPECT_FALSE (std::is_copy_constructible_v<RegisterField<Field4_3>>);
    EXPECT_FALSE (std::is_copy_assignable_v<RegisterField<Field4_3>>);
}

TEST (RegisterFieldTest, readReturnsFieldBits)
{
    u16 value {0123456};
    RegisterField<Field0_4> field0_4 {value};
    RegisterField<Field4_3> field4_3 {value};
    RegisterField<Field13_3> field13_3 {value};

    EXPECT_EQ (field0_4, 016);
    EXPECT_EQ (field4_3, 02);
    EXPECT_EQ (field13_3, 05);
}

TEST (RegisterFieldTest, readDoesNotChangeValue)
{
    u16 value {0123456};
    RegisterField<Field4_3> field {value};

    u16 fieldValue = field;
    (void) fieldValue;
    EXPECT_EQ (value, 0123456);
}

TEST (RegisterFieldTest, writeSetsOnlyFieldBits)
{
    u16 value {0};
    RegisterField<Field4_3> field {value};

    field = 07;
    EXPECT_EQ (value, 0000160);
    EXPECT_EQ (field, 07);
}

TEST (RegisterFieldTest, writePreservesOtherBits)
{
    u16 value {0177777};
    RegisterField<Field4_3> field {value};

    field = 0;
    EXPECT_EQ (value, 0177617);

    field = 02;
    EXPECT_EQ (value, 0177657);
}

TEST (RegisterFieldTest, writeTruncatesToFieldWidth)
{
    u16 value {0};
    RegisterField<Field4_3> field {value};

    // Only the three least significant bits of 012 (= 010 + 02) fit
    field = 012;
    EXPECT_EQ (field, 02);
    EXPECT_EQ (value, 0000040);
}

TEST (RegisterFieldTest, writeReturnsField)
{
    u16 value {0};
    RegisterField<Field4_3> field {value};

    EXPECT_EQ (&(field = 03), &field);
}

TEST (RegisterFieldTest, fieldReflectsChangedRegisterValue)
{
    u16 value {0};
    RegisterField<Field13_3> field {value};

    value = 0060000;
    EXPECT_EQ (field, 03);
}

TEST (RegisterFieldTest, fieldsShareRegisterValue)
{
    u16 value {0};
    RegisterField<Field0_4> field0_4 {value};
    RegisterField<Field4_3> field4_3 {value};
    RegisterField<Bit15> bit15 {value};

    field0_4 = 012;
    field4_3 = 05;
    bit15 = 1;

    EXPECT_EQ (value, 0100132);
    EXPECT_EQ (field0_4, 012);
    EXPECT_EQ (field4_3, 05);
    EXPECT_EQ (bit15, 1);
}

TEST (RegisterFieldTest, singleBitFieldCanBeUsedAsBool)
{
    u16 value {0};
    RegisterField<Bit5> bit5 {value};

    EXPECT_FALSE (bit5);

    bit5 = true;
    EXPECT_TRUE (bit5);
    EXPECT_EQ (value, 0000040);

    bit5 = false;
    EXPECT_FALSE (bit5);
    EXPECT_EQ (value, 0);
}

TEST (RegisterFieldTest, unaryPlusReturnsBitsAtPosition)
{
    u16 value {0177777};
    RegisterField<Field4_3> field4_3 {value};
    RegisterField<Field13_3> field13_3 {value};

    EXPECT_EQ (+field4_3, 0000160);
    EXPECT_EQ (+field13_3, 0160000);

    field4_3 = 02;
    EXPECT_EQ (+field4_3, 0000040);
}

TEST (RegisterFieldTest, preIncrementIncrementsField)
{
    u16 value {0100000};
    RegisterField<Field4_3> field {value};

    EXPECT_EQ (++field, 01);
    EXPECT_EQ (value, 0100020);
}

TEST (RegisterFieldTest, postIncrementReturnsOldValue)
{
    u16 value {0};
    RegisterField<Field4_3> field {value};

    field = 03;
    EXPECT_EQ (field++, 03);
    EXPECT_EQ (field, 04);
}

TEST (RegisterFieldTest, incrementWrapsAroundWithinField)
{
    u16 value {0177777};
    RegisterField<Field4_3> field {value};

    ++field;
    EXPECT_EQ (field, 0);
    EXPECT_EQ (value, 0177617);
}

TEST (RegisterFieldTest, preDecrementDecrementsField)
{
    u16 value {0};
    RegisterField<Field4_3> field {value};

    field = 05;
    EXPECT_EQ (--field, 04);
}

TEST (RegisterFieldTest, postDecrementReturnsOldValue)
{
    u16 value {0};
    RegisterField<Field4_3> field {value};

    field = 05;
    EXPECT_EQ (field--, 05);
    EXPECT_EQ (field, 04);
}

TEST (RegisterFieldTest, decrementWrapsAroundWithinField)
{
    u16 value {0100000};
    RegisterField<Field4_3> field {value};

    --field;
    EXPECT_EQ (field, 07);
    EXPECT_EQ (value, 0100160);
}

TEST (RegisterFieldTest, fullWidthFieldCoversRegister)
{
    u16 value {0};
    RegisterField<Field0_16> field {value};

    field = 0123456;
    EXPECT_EQ (value, 0123456);
    EXPECT_EQ (field, 0123456);
}
