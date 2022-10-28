#include "bitmask.h"

#include <gtest/gtest.h>

// Definition of the test fixture
class BitmaskTest : public ::testing::Test
{
public:
    enum class Cars 
    {
        Simca,
        Vauxhall,
        Citroen,
        Ford,
        Mazda,
        _
    };
};

TEST_F (BitmaskTest, defaultValueIsFalse)
{
    Bitmask<Cars> cars;
    
    EXPECT_FALSE (cars & Cars::Simca);
    EXPECT_FALSE (cars & Cars::Vauxhall);
    EXPECT_FALSE (cars & Cars::Citroen);
    EXPECT_FALSE (cars & Cars::Ford);
    EXPECT_FALSE (cars & Cars::Mazda);
}

TEST_F (BitmaskTest, bitsCanBeSet)
{
    Bitmask<Cars> cars;
    cars = Cars::Simca | Cars::Vauxhall;
    EXPECT_TRUE (cars & Cars::Simca);
    EXPECT_TRUE (cars & Cars::Vauxhall);
    EXPECT_FALSE (cars & Cars::Citroen);
    EXPECT_FALSE (cars & Cars::Ford);
    EXPECT_FALSE (cars & Cars::Mazda);
}

TEST_F (BitmaskTest, bitsCanBeComplemented)
{
    Bitmask<Cars> cars;
    cars = Cars::Simca | Cars::Vauxhall;
    cars = ~cars;

    EXPECT_FALSE (cars & Cars::Simca);
    EXPECT_FALSE (cars & Cars::Vauxhall);
    EXPECT_TRUE (cars & Cars::Citroen);
    EXPECT_TRUE (cars & Cars::Ford);
    EXPECT_TRUE (cars & Cars::Mazda);
}

TEST_F (BitmaskTest, enumCanBeComplemented)
{
    Bitmask<Cars> cars;
    cars = Cars::Simca | Cars::Vauxhall;
    cars = ~Bitmask(Cars::Simca);

    EXPECT_FALSE (cars & Cars::Simca);
    EXPECT_TRUE (cars & Cars::Vauxhall);
    EXPECT_TRUE (cars & Cars::Citroen);
    EXPECT_TRUE (cars & Cars::Ford);
    EXPECT_TRUE (cars & Cars::Mazda);
}

TEST_F (BitmaskTest, bitCanBeCleared1)
{
    Bitmask<Cars> cars;
    cars = Cars::Simca | Cars::Vauxhall;
    cars &= Cars::Vauxhall | Cars::Citroen | Cars::Ford | Cars::Mazda;

    EXPECT_FALSE (cars & Cars::Simca);
    EXPECT_TRUE (cars & Cars::Vauxhall);
    EXPECT_FALSE (cars & Cars::Citroen);
    EXPECT_FALSE (cars & Cars::Ford);
    EXPECT_FALSE (cars & Cars::Mazda);
}

TEST_F (BitmaskTest, bitCanBeCleared2)
{
    Bitmask<Cars> cars;
    cars = Cars::Simca | Cars::Vauxhall;
    cars &= ~Bitmask(Cars::Simca);

    EXPECT_FALSE (cars & Cars::Simca);
    EXPECT_TRUE (cars & Cars::Vauxhall);
    EXPECT_FALSE (cars & Cars::Citroen);
    EXPECT_FALSE (cars & Cars::Ford);
    EXPECT_FALSE (cars & Cars::Mazda);
}

TEST_F (BitmaskTest, bitsCanBeTested1)
{
    Bitmask<Cars> cars;
    cars = Cars::Simca | Cars::Vauxhall;

    EXPECT_TRUE(cars & Bitmask(Cars::Simca | Cars::Vauxhall));
}

TEST_F (BitmaskTest, bitsCanBeTested2)
{
    Bitmask<Cars> cars;
    cars = Cars::Simca | Cars::Vauxhall;

    EXPECT_TRUE(cars == Bitmask(Cars::Simca | Cars::Vauxhall));
    EXPECT_FALSE(cars == Bitmask(Cars::Simca | Cars::Vauxhall |
        Cars::Citroen));
}