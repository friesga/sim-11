#include "fio/fio.h"

#include <gtest/gtest.h>

// This unit test is not meant as an exhaustive test of the fio functions
// as these are trivial, it just checks some basic usage scenario's.
//
TEST (FioTest, fileCanBeCreated)
{
    FILE* filePtr = fio::fopen ("test.txt", "w");
    EXPECT_TRUE (filePtr != nullptr);

    EXPECT_EQ (fio::fsize (filePtr), 0);
    EXPECT_EQ (fio::ftell (filePtr), 0);
    fclose (filePtr);
}

TEST (FioTest, fileCanBeWritten)
{
    FILE* filePtr = fio::fopen ("test.txt", "w");
    EXPECT_TRUE (filePtr != nullptr);

    char* buffer = new char[1000];

    // Write 1000 bytes
    EXPECT_EQ (fio::fwrite (buffer, 1, 1000, filePtr), 1000);

    EXPECT_EQ (fio::fsize (filePtr), 1000);
    EXPECT_EQ (fio::ftell (filePtr), 1000);
    fclose (filePtr);

    delete[] buffer;
}

TEST (FioTest, fileCanBeRead)
{
    FILE* filePtr = fio::fopen ("test.txt", "w+");
    EXPECT_TRUE (filePtr != nullptr);

    char* buffer = new char[1000];

    // Write 1000 bytes
    EXPECT_EQ (fio::fwrite (buffer, 1, 1000, filePtr), 1000);

    // Seek to the beginning of the file
    EXPECT_EQ (fio::fseek (filePtr, 0, SEEK_SET), 0);
    EXPECT_EQ (fio::ftell (filePtr), 0);

    EXPECT_EQ (fio::fread (&buffer[0], 1, 1000, filePtr), 1000);
    fclose (filePtr);

    delete[] buffer;
}