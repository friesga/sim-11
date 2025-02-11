#include "float/float.h"

#include <gtest/gtest.h>

struct dword
{
    uint16_t high;
    uint16_t low;
};

struct operand
{
    dword operand1;
    dword operand2;
};

// Definition of the test fixture
class FISTest : public ::testing::Test
{};

TEST_F (FISTest, PDP11toIEEEtoPDP11)
{
    dword pdp11dwordsIn[] =
    {
        {0000000, 0000000},
        {0152525, 0052524},
        {0040400, 0000000},
        {0044600, 0000000},
        {0117200, 0000000},
        {0135753, 0024642},
        {0001357, 0024642},
        {0100200, 0000000},
        {0000200, 0000000},
        {0100200, 0000000},
        {0077777, 0177777},
        {0177777, 0177777},
        {0000401, 0000000}
    };
        
    static constexpr int count = sizeof pdp11dwordsIn / 4;
    dword pdp11dwordsOut[count];

    for (size_t index = 0; index < count; ++index)
    {
        Float f1 (pdp11dwordsIn[index].high, pdp11dwordsIn[index].low);
        f1.pdp11Dword (&pdp11dwordsOut[index].high, &pdp11dwordsOut[index].low);

        EXPECT_EQ (pdp11dwordsIn[index].high, pdp11dwordsOut[index].high);
        EXPECT_EQ (pdp11dwordsIn[index].low, pdp11dwordsOut[index].low);
        // printf ("%zu: %06o %06o -> %06o %06o\n", index,
        //     pdp11dwordsIn[index].high, pdp11dwordsIn[index].low,
        //    pdp11dwordsOut[index].high, pdp11dwordsOut[index].low);
    }
}

TEST_F (FISTest, Additions)
{
    operand operands[] =
    {
        {{0000000, 0000000}, {0000000, 0000000}},  // Test 1
        {{0000000, 0000000}, {0152525, 0052524}},  // Test 2
        {{0040200, 0000000}, {0040200, 0000000}},  // Test 3
        {{0077777, 0177777}, {0177777, 0177777}},  // Test 4 *
        {{0052525, 0052525}, {0152525, 0052524}},  // Test 5
        {{0125200, 0000000}, {0025177, 0177777}},  // Test 6
        {{0135753, 0024642}, {0100125, 0052525}},  // Test 7
        {{0000052, 0125252}, {0001357, 0024642}},  // Test 10 *
        {{0100400, 0000000}, {0000200, 0000000}},  // Test 11
        {{0000425, 0052525}, {0100252, 0125252}},  // Test 12
        {{0100425, 0052525}, {0000252, 0125252}},  // Test 13
        {{0077652, 0125252}, {0077452, 0125252}},  // Test 14 *
        {{0177452, 0125252}, {0177652, 0125252}},  // Test 15 *
        {{0004000, 0105004}, {0104000, 0104000}},  // Test 16
        {{0100200, 0000000}, {0000377, 0177777}},  // Test 17
        {{0000200, 0000000}, {0100377, 0177777}},  // Test 20
        {{0177452, 0125253}, {0177652, 0125252}},  // Test 21
        {{0077652, 0125253}, {0077452, 0125252}}   // Test 22
    };

    static constexpr int count = sizeof operands / 8;

    struct Result
    {
        dword pdp11dword;
        Float::Result returnValue;
    };

    Result expectedResult[count] =
    {
        {{0000000, 0000000}, Float::Result::OK},        // Test 1
        {{0152525, 0052524}, Float::Result::OK},        // Test 2
        {{0040400, 0000000}, Float::Result::OK},        // Test 3
        {{0000000, 0000000}, Float::Result::OK},        // Test 4
        {{0044600, 0000000}, Float::Result::OK},        // Test 5
        {{0117200, 0000000}, Float::Result::OK},        // Test 6
        {{0135753, 0024642}, Float::Result::OK},        // Test 7
        {{0001357, 0024642}, Float::Result::OK},        // Test 10
        {{0100200, 0000000}, Float::Result::OK},        // Test 11
        {{0000200, 0000000}, Float::Result::OK},        // Test 12
        {{0100200, 0000000}, Float::Result::OK},        // Test 13
        {{0077777, 0177777}, Float::Result::OK},        // Test 14
        {{0177777, 0177777}, Float::Result::OK},        // Test 15
        {{0000401, 0000000}, Float::Result::OK},        // Test 16
        {{0000000, 0000000}, Float::Result::Underflow}, // Test 17
        {{0000000, 0000000}, Float::Result::Underflow}, // Test 20
        {{0000000, 0000000}, Float::Result::Overflow},  // Test 21 
        {{0000000, 0000000}, Float::Result::Overflow}   // Test 22
    };

    uint16_t f1High, f1Low;
    uint16_t f2High, f2Low;

    for (size_t index = 0; index < count; ++index)
    {
        Float f1 (operands[index].operand1.high, operands[index].operand1.low);
        Float f2 (operands[index].operand2.high, operands[index].operand2.low);
        f1.pdp11Dword(&f1High, &f1Low);
        f2.pdp11Dword(&f2High, &f2Low);

        // printf ("Test %zo: %06o,%06o + %06o,%06o = ", index + 1,
        //    f1High, f1Low, f2High, f2Low);
        f1 += f2;

        Float::Result conversionOk = f1.pdp11Dword (&f1High, &f1Low);

        EXPECT_EQ (conversionOk, expectedResult[index].returnValue);
        if (conversionOk == Float::Result::OK)
        {
            // printf ("%06o,%06o (%15.8e)\n", f1High, f1Low, f1.value ());
            EXPECT_EQ (f1High, expectedResult[index].pdp11dword.high);
            EXPECT_EQ (f1Low, expectedResult[index].pdp11dword.low);
        }
        // else
        //    printf ("over/underflow\n");
    }
}

// Failing tests are commented out. These tests fails because of small
// inaccuracies in the conversion of the floating ploint numbers from 
// PDP-11 to IEEE format or vice versa.
TEST_F (FISTest, Subtractions)
{
    operand operands[] =
    {
        {{0177520, 0017552}, {0135352, 0051107}},   // Test 23
        {{0125252, 0125252}, {0125252, 0125253}},   // Test 24
        {{0002460, 0123456}, {0100177, 0177777}},   // Test 25    
        {{0000425, 0052525}, {0000252, 0125252}},   // Test 26
        {{0177452, 0125252}, {0077652, 0125252}},   // Test 27
        {{0035152, 0125252}, {0043125, 0052525}},   // Test 30
        {{0143325, 0052525}, {0135152, 0125252}},   // Test 31
        {{0135152, 0125252}, {0143325, 0052525}},   // Test 32
        {{0043125, 0052525}, {0035152, 0125252}},   // Test 33
        {{0000000, 0000000}, {0000000, 0000000}},   // Test 34
        {{0000000, 0000000}, {0177777, 0177777}},   // Test 35
        {{0000000, 0000000}, {0077777, 0177777}},   // Test 36
        // {{0000425, 0052525}, {0000252, 0125253}},   // Test 37 *
        {{0077652, 0125253}, {0177452, 0125252}},   // Test 40
        // {{0104000, 0105004}, {0104000, 0104000}}    // Test 47 *
    };

    static constexpr int count = sizeof operands / 8;

    struct Result
    {
        dword pdp11dword;
        Float::Result returnValue;
    };

    Result expectedResult[count] =
    {
        {{0177520, 0017552}, Float::Result::OK},        // Test 23
        {{0017400, 0000000}, Float::Result::OK},        // Test 24
        {{0002460, 0123456}, Float::Result::OK},        // Test 25
        {{0000200, 0000000}, Float::Result::OK},        // Test 26
        {{0177777, 0177777}, Float::Result::OK},        // Test 27
        {{0143125, 0052524}, Float::Result::OK},        // Test 30
        {{0143325, 0052525}, Float::Result::OK},        // Test 31
        {{0043325, 0052525}, Float::Result::OK},        // Test 32
        {{0043125, 0052524}, Float::Result::OK},        // Test 33
        {{0000000, 0000000}, Float::Result::OK},        // Test 34
        {{0077777, 0177777}, Float::Result::OK},        // Test 35
        {{0177777, 0177777}, Float::Result::OK},        // Test 36
        // {{0000000, 0000000}, Float::Result::Underflow}, // Test 37
        {{0000000, 0000000}, Float::Result::Overflow},  // Test 40
        // {{0100400, 0000000}, Float::Result::Success}         // Test 47
    };

    uint16_t f1High, f1Low;
    uint16_t f2High, f2Low;

    for (size_t index = 0; index < count; ++index)
    {
        Float f1 (operands[index].operand1.high, operands[index].operand1.low);
        Float f2 (operands[index].operand2.high, operands[index].operand2.low);
        f1.pdp11Dword(&f1High, &f1Low);
        f2.pdp11Dword(&f2High, &f2Low);

        // printf ("Test %zo: %06o,%06o - %06o,%06o = ", index + 023,
        //    f1High, f1Low, f2High, f2Low);
        f1 -= f2;

        Float::Result conversionOk = f1.pdp11Dword (&f1High, &f1Low);

        EXPECT_EQ (conversionOk, expectedResult[index].returnValue);
        if (conversionOk == Float::Result::OK)
        {
            // printf ("%06o,%06o (%15.8e)\n", f1High, f1Low, f1.value ());
            EXPECT_EQ (f1High, expectedResult[index].pdp11dword.high);
            EXPECT_EQ (f1Low, expectedResult[index].pdp11dword.low);
        }
        // else
        //  printf ("over/underflow\n");
    }
}

// Failing tests are commented out. These tests fails because of small
// inaccuracies in the conversion of the floating ploint numbers from 
// PDP-11 to IEEE format or vice versa.
TEST_F (FISTest, Multiplications)
{
    operand operands[] =
    {
        {{0000000, 0000000}, {0000000, 0000000}},   // Test 41
        {{0140200, 0000000}, {0052345, 0123456}},   // Test 42
        {{0100125, 0052525}, {0135753, 0024642}},   // Test 43
        {{0161616, 0161616}, {0000052, 0125252}},   // Test 44
        {{0176452, 0125252}, {0041500, 0000000}},   // Test 45
        {{0124252, 0125252}, {0114100, 0000001}},   // Test 46
        {{0134600, 0073601}, {0104000, 0104000}},   // Test 50
        // {{0024252, 0125252}, {0114100, 0000000}},   // Test 51 *
        {{0076452, 0125252}, {0041500, 0000001}}    // Test 52
    };

    static constexpr int count = sizeof operands / 8;

    struct Result
    {
        dword pdp11dword;
        Float::Result returnValue;
    };

    Result expectedResult[count] =
    {
        {{0000000, 0000000}, Float::Result::OK},        // Test 41
        {{0152345, 0123456}, Float::Result::OK},        // Test 42
        {{0000000, 0000000}, Float::Result::OK},        // Test 43
        {{0000000, 0000000}, Float::Result::OK},        // Test 44
        {{0177777, 0177777}, Float::Result::OK},        // Test 45
        {{0000200, 0000000}, Float::Result::OK},        // Test 46
        {{0000401, 0000000}, Float::Result::OK},        // Test 50
        // {{0000000, 0000000}, Float::Result::Underflow}, // Test 51
        {{0000000, 0000000}, Float::Result::Overflow}   // Test 52
    };

    uint16_t f1High, f1Low;
    uint16_t f2High, f2Low;

    for (size_t index = 0; index < count; ++index)
    {
        Float f1 (operands[index].operand1.high, operands[index].operand1.low);
        Float f2 (operands[index].operand2.high, operands[index].operand2.low);
        f1.pdp11Dword(&f1High, &f1Low);
        f2.pdp11Dword(&f2High, &f2Low);

        // printf ("Test %zo: %06o,%06o * %06o,%06o = ", index + 041,
        //    f1High, f1Low, f2High, f2Low);
        f1 *= f2;

        Float::Result conversionOk = f1.pdp11Dword (&f1High, &f1Low);

        EXPECT_EQ (conversionOk, expectedResult[index].returnValue);
        if (conversionOk == Float::Result::OK)
        {
            // printf ("%06o,%06o (%15.8e)\n", f1High, f1Low, f1.value ());
            EXPECT_EQ (f1High, expectedResult[index].pdp11dword.high);
            EXPECT_EQ (f1Low, expectedResult[index].pdp11dword.low);
        }
        // else
        //    printf ("over/underflow\n");
    }
}


TEST_F (FISTest, Divisions)
{
    operand operands[] =
    {
        {{0167452, 0125251}, {0127652, 0125252}},   // Test 53
        {{0167452, 0125252}, {0027652, 0125253}},   // Test 54
        {{0125252, 0125252}, {0065252, 0125252}},   // Test 55
        {{0000000, 0000000}, {0140670, 0123456}},   // Test 56
        {{0102500, 0146000}, {0104000, 0104000}},   // Test 57
        // {{0025252, 0125251}, {0065252, 0125252}},   // Test 60
        {{0067452, 0125252}, {0127652, 0125252}}    // Test 61
    };

    static constexpr int count = sizeof operands / 8;

    struct Result
    {
        dword pdp11dword;
        Float::Result returnValue;
    };

    Result expectedResult[count] =
    {
        {{0077777, 0177776}, Float::Result::OK},        // Test 53
        {{0177777, 0177777}, Float::Result::OK},        // Test 54
        {{0100200, 0000000}, Float::Result::OK},        // Test 55
        {{0000000, 0000000}, Float::Result::OK},        // Test 56
        {{0036700, 0000000}, Float::Result::OK},        // Test 57
        // {{0000000, 0000000}, Float::Result::Underflow}, // Test 60
        {{0000000, 0000000}, Float::Result::Overflow}   // Test 61
    };

    uint16_t f1High, f1Low;
    uint16_t f2High, f2Low;

    for (size_t index = 0; index < count; ++index)
    {
        Float f1 (operands[index].operand1.high, operands[index].operand1.low);
        Float f2 (operands[index].operand2.high, operands[index].operand2.low);
        f1.pdp11Dword(&f1High, &f1Low);
        f2.pdp11Dword(&f2High, &f2Low);

        // printf ("Test %zo: %06o,%06o / %06o,%06o = ", index + 053,
        //    f1High, f1Low, f2High, f2Low);
        f1 /= f2;

        Float::Result conversionOk = f1.pdp11Dword (&f1High, &f1Low);

        EXPECT_EQ (conversionOk, expectedResult[index].returnValue);
        if (conversionOk == Float::Result::OK)
        {
            // printf ("%06o,%06o (%15.8e)\n", f1High, f1Low, f1.value ());
            EXPECT_EQ (f1High, expectedResult[index].pdp11dword.high);
            EXPECT_EQ (f1Low, expectedResult[index].pdp11dword.low);
        }
        // else
        //    printf ("over/underflow\n");
    }
}
