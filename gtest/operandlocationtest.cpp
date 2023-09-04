#include "kd11_na/cpu/operandlocation/operandlocation.h"
#include "dummycpu/dummycpu.h"

#include <gtest/gtest.h>
#include <string>

using std::string;

TEST (OperandLocationTest, InitialOperandLocationIsInvalid)
{
    OperandLocation operandLocation {};
    EXPECT_FALSE (operandLocation.isValid ());
}

TEST (OperandLocationTest, InitialRegisterOperandLocationIsValid)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {RegisterOperandLocation {&dummyCpu, (u8) 1}};
    EXPECT_TRUE (operandLocation.isValid ());
}

TEST (OperandLocationTest, InitialMemoryOperandLocationIsValid)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {MemoryOperandLocation {&dummyCpu, 100}};
    EXPECT_TRUE (operandLocation.isValid ());
}

TEST (OperandLocationTest, InitialMemoryOperandLocationIsInValid)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {MemoryOperandLocation {&dummyCpu, CondData<u16> {}}};
    EXPECT_FALSE (operandLocation.isValid ());
}

TEST (OperandLocationTest, RegisterOperandLocationCanBeAssignedToOperandLocation)
{
    DummyCpu dummyCpu;
    
    RegisterOperandLocation registerOperandLocation {&dummyCpu, 1};
    OperandLocation operandLocation;
    operandLocation = registerOperandLocation;
    EXPECT_TRUE (operandLocation.write<u16> (0177777));

    EXPECT_EQ (dummyCpu.registers () [1], 0177777);
}

TEST (OperandLocationTest, MemoryOperandLocationCanBeAssignedToOperandLocation)
{
    DummyCpu dummyCpu;
    
    MemoryOperandLocation memoryOperandLocation {&dummyCpu, 100};
    OperandLocation operandLocation;
    operandLocation = memoryOperandLocation;
    EXPECT_TRUE (operandLocation.write<u16> (0177777));

    EXPECT_EQ (dummyCpu.fetchWord (100), 0177777);
}

// Verify that a write to a register operand location results in writing
// the specified value to the specified register.
TEST (OperandLocationTest, RegisterLocationWordCanBeWritten)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {RegisterOperandLocation {&dummyCpu, 1}};
    EXPECT_TRUE (operandLocation.write<u16> (0177777));

    EXPECT_EQ (dummyCpu.registers () [1], 0177777);
}

// Verify that the correct value is read from a register operand location
TEST (OperandLocationTest, RegisterLocationWordCanBeRead)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {RegisterOperandLocation {&dummyCpu, 1}};
   
    dummyCpu.registers() [1] = 0177777;

    EXPECT_EQ (operandLocation.contents<CondData<u16>> ().value (), 0177777);
}

TEST (OperandLocationTest, MemoryLocationWordCanBeWritten)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {MemoryOperandLocation {&dummyCpu, CondData<u16> {100}}};
    EXPECT_TRUE (operandLocation.write<u16> (1000));

    EXPECT_EQ (operandLocation.contents<CondData<u16>> ().value (), 1000);
    EXPECT_EQ (dummyCpu.fetchWord (100), 1000);
}

TEST (OperandLocationTest, RegisterLocationByteCanBeWritten)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {RegisterOperandLocation {&dummyCpu, 1}};
    EXPECT_TRUE (operandLocation.write<u8> (0377));

    EXPECT_EQ (dummyCpu.registers () [1] & 0377, 0377);
}

TEST (OperandLocationTest, RegisterLocationByteCanBeRead)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {RegisterOperandLocation {&dummyCpu, 1}};
   
    dummyCpu.registers() [1] = 0101;

    EXPECT_EQ (operandLocation.contents<CondData<u8>> ().value (), 0101);
}

TEST (OperandLocationTest, MemoryLocationByteCanBeWrittenAndRead)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {MemoryOperandLocation {&dummyCpu, CondData<u16> {100}}};
    EXPECT_TRUE (operandLocation.write<u8> (255));

    EXPECT_EQ (operandLocation.contents<CondData<u8>> ().value (), 255);
    EXPECT_EQ (dummyCpu.fetchWord (100), 255);
}

TEST (OperandLocationTest, WriteToInvalidMemoryOperationLocationFails)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {MemoryOperandLocation {&dummyCpu, CondData<u16> {65534}}};
    EXPECT_FALSE (operandLocation.write<u16> (255));
}

TEST (OperandLocationTest, WriteToInvalidOperationLocationFails)
{
    OperandLocation operandLocation {};
    EXPECT_THROW (operandLocation.write<u16> (255), string);
}

TEST (OperandLocationTest, ReadFromInvalidOperationLocationFails)
{
    OperandLocation operandLocation {};
    EXPECT_THROW (operandLocation.contents<CondData<u16>> ().value (), string);
}

TEST (OperandLocationTest, RegisterLocationCanBeConvertedTou16)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {RegisterOperandLocation {&dummyCpu, 1}};
    
    u16 regNr = operandLocation;
    EXPECT_EQ (regNr, 1);
}

TEST (OperandLocationTest, MemoryLocationCanBeConvertedTou16)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {MemoryOperandLocation {&dummyCpu, CondData<u16> {100}}};
    
    u16 memoryAddress = operandLocation;
    EXPECT_EQ (memoryAddress, 100);
}

TEST (OperandLocationTest, TypeOfMemoryLocationCanBeDetermined)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {MemoryOperandLocation {&dummyCpu, CondData<u16> {100}}};

    EXPECT_TRUE (operandLocation.isA<MemoryOperandLocation> ());
    EXPECT_FALSE (operandLocation.isA<RegisterOperandLocation> ());
}

TEST (OperandLocationTest, TypeOfRegisterLocationCanBeDetermined)
{
    DummyCpu dummyCpu;
    OperandLocation operandLocation {RegisterOperandLocation {&dummyCpu, 1}};

    EXPECT_TRUE (operandLocation.isA<RegisterOperandLocation> ());
    EXPECT_FALSE (operandLocation.isA<MemoryOperandLocation> ());
}
