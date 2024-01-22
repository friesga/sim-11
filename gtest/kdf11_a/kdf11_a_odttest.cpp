#include "dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "kd/kdf11_a/odt/kdf11_a_odt.h"
#include "kd/kdf11_a/cpudata/kdf11_acpudata.h"
#include "kd/kdf11_a/ktf11_a/ktf11_a.h"
#include "../testconsoleaccess.h"

#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <string>
#include <vector>
#include <iostream>

using std::unique_ptr;
using std::make_unique;
using std::move;
using std::string;
using std::vector;

#define EXPECT_CHAR(c) \
    { \
        char got = testConsole->getCharacter (); \
        EXPECT_TRUE (c == got) << sequence.description << \
            " expected: " << c << " got: " << got << '\n'; \
    }

class KDF11_A_ODTTest : public ::testing::Test
{
protected:
    struct TestSequence
    {
        string input;
        string expectedOutput;
        string description;
    };

    void executeSequence (TestSequence sequence)
    {
        unique_ptr<ConsoleAccess> console = 
            make_unique<TestConsoleAccess> (sequence.input);

        // 'Steal' a raw pointer from the unique_ptr to be able to send and
        // receive characters to and from the TestConsole as the unique_ptr
        // will be invalid after being moved to KD11ODT.
        TestConsoleAccess *testConsole = static_cast<TestConsoleAccess*> (console.get ());

        Qbus bus;
        KDF11_ACpuData cpuData;
        KTF11_A mmu {&bus, &cpuData};
        KD11_NA_CpuControl kd11cpu (&bus, &cpuData, &mmu);
        MSV11D msv11d (&bus);
        bus.installModule (1, &msv11d);

        // Create a KD11ODT instance and let it process a character sequence
        KDF11_A_ODT kd11odt {&bus, &cpuData, &kd11cpu, &mmu, move (console)};

        // Read the characters from the input sequence and feed them to ODT.
        // The characters could be retrieved from the input sequence directly,
        // but the detour via the test console is necessary as KD11ODT in the
        // binary dump command accesses the console.
        while (testConsole->available ())
            kd11odt.processCharacter (testConsole->read ());

        // Compare result from the DLV11J with the expected characters
        for (char c : sequence.expectedOutput)
            EXPECT_CHAR (c);
    }
};

// Verify that KD11ODT reacts as expected on the given input string.
//
// This test comprises the commands implemented in both the KD11-F (LSI-11)
// and the the KDF11 (11/23) processors.
// 
// The Qbus, KD11CPU, TestConsole, DLV11J and KD11ODT all have to be
// initialized at the start of processing the input string and the input
// string has to be passed to the TestConsole and DLV11J before KD11ODT
// starts processing the characters in the string.
//
// Note that the default value of R7. i.e. the PC will be the boot address
// 0000000.
//
TEST_F (KDF11_A_ODTTest, KDF11PlusKD11_FCommands)
{
    vector<TestSequence> testSequences =
    {
        {"P",                  "\n000000\n@P",
              "Exit with Proceed"},
        {"9P",                 "\n000000\n@9?\n@",
              "Error on invalid char at prompt"},
        {"19P",                "\n000000\n@19?\n@",
              "Error while entering address"},
        {"10\rP",              "\n000000\n@10\r\n@",
            "<CR> while entering address accepted"},
        {"R0\rP",              "\n000000\n@R0\r\n@",
            "<CR> while entering register number accepted"},
        {"R\rP",              "\n000000\n@R\r?\n@",
            "Error on <CR> after register indicator"},
        {"10/\rP",             "\n000000\n@10/000000 \r\n@",
            "Address can be opened and closed"},
        {"10/2\r10/\rP",       "\n000000\n@10/000000 2\r\n@10/000002 \r\n@",
            "New address value can be entered"},
        {"10/9\rP",            "\n000000\n@10/000000 9?\n@",
            "Error on invalid char at open address"},
        {"10/19P",             "\n000000\n@10/000000 19?\n@",
            "Error on invalid char at new address value"},
        {"10/\n\rP",           "\n000000\n@10/000000 \n000012/000000 \r\n@",
            "<LF> opens next location"},
        {"R0/\rP",             "\n000000\n@R0/000000 \r\n@",
            "R starts register entry"},
        {"$0/\rP",             "\n000000\n@$0/000000 \r\n@",
            "$ starts register entry"},
        {"RS/\rP",             "\n000000\n@RS/000000 \r\n@",
            "RS starts PSW entry"},
        {"R077/\rP",             "\n000000\n@R077/000000 \r\n@",
            "R077 starts PSW entry"},
        {"R477/\rP",             "\n000000\n@R477/000000 \r\n@",
            "R477 starts PSW entry"},
        {"R0/10\rR0/\rP",      "\n000000\n@R0/000000 10\r\n@R0/000010 \r\n@",
            "New register value can be entered"},
        {"RS/10\rRS/\rP",      "\n000000\n@RS/000000 10\r\n@RS/000010 \r\n@",
            "New PSW value can be entered"},
        {"RS/21\rRS/\rP",      "\n000000\n@RS/000000 21\r\n@RS/000001 \r\n@",
            "PSW T-bit cannot be changed"},
        {"R0/19P",             "\n000000\n@R0/000000 19?\n@",
            "Error on invalid char at new register value"},
        {"R0/10\rR20/\rP", "\n000000\n@R0/000000 10\r\n@R20/000010 \r\n@",
            "Last register digit used"},
        {"R9\rP",              "\n000000\n@R9?\n@",
            "Error on invalid char at register entry"},
        {"R019\rP",            "\n000000\n@R019?\n@",
            "Error on invalid char while entering register"},
        {"R0/177777\rR0/\rP",  "\n000000\n@R0/000000 177777\r\n@R0/177777 \r\n@",
            "New register value can be entered"},
        {"R0/2177777\rR0/\rP",       "\n000000\n@R0/000000 2177777\r\n@R0/177777 \r\n@",
            "16-bit value used as new register value"},
        {"R7/\n\rP",           "\n000000\n@R7/000000 \nR0/000000 \r\n@",
            "<LF> opens next register and rolls over to R0"},
        {"RS/\n\rP",           "\n000000\n@RS/000000 \n@",
            "<LF> on opened PSW closes location"},
        {"R0/10\n\rR0/\rP",    "\n000000\n@R0/000000 10\nR1/000000 \r\n@R0/000010 \r\n@",
            "New register value can be entered with <LF>"},
        {"0/10\n\r0/\rP",      "\n000000\n@0/000000 10\n000002/000000 \r\n@0/000010 \r\n@",
            "New address value can be entered with <LF>"},
        {"RS/10\nRS/\rP",      "\n000000\n@RS/000000 10\n@RS/000010 \r\n@",
            "New PSW value can be entered with <LF>"},
        {"10/1012345\r10/\rP", "\n000000\n@10/000000 1012345\r\n@10/012345 \r\n@",
            "Last 6 digits determine new address value"},
        {"R0/1012345\rR0/\rP", "\n000000\n@R0/000000 1012345\r\n@R0/012345 \r\n@",
            "Last 6 digits determine new register value"},
        {"100000010/2\r10/\rP","\n000000\n@100000010/000000 2\r\n@10/000002 \r\n@",
            "Least significant 16 bits determine address"},
        {"0/177777\r1/\rP",  "\n000000\n@0/000000 177777\r\n@1/177777 \r\n@",
            "Clear least significant bit on odd address"},
        {"/P",               "\n000000\n@/?\n@",
            "Error on / with no location open"},
        {"10/2\r/\rP",       "\n000000\n@10/000000 2\r\n@/000002 \r\n@",
            "Address location can be reopened"},
        {"R0/2\r/\rP",       "\n000000\n@R0/000000 2\r\n@/000002 \r\n@",
            "Register location can be reopened"},
        {"RS/2\r/\rP",       "\n000000\n@RS/000000 2\r\n@/000002 \r\n@",
            "PSW location can be reopened"},
        {"G",                "\n000000\n@G",
            "Go command with default address accepted"},
        {"200G",             "\n000000\n@200G",
            "Go command with address accepted"},
        {"777774/P",         "\n000000\n@777774/?\n@",
            "Error on opening invalid address"},
        {"0/\r777774//\rP", "\n000000\n@0/000000 \r\n@777774/?\n@/?\n@",
            "Last opened location set on opening invalid address"},
        {"177776/\nP",       "\n000000\n@177776/000000 \n200000/?\n@",
            "Error on next opened invalid address"},
        {"11/2\r10/\rP",       "\n000000\n@11/000000 2\r\n@10/000002 \r\n@",
            "Byte address value can be opened and value can be set"},
        {"11/\n\rP",           "\n000000\n@11/000000 \n000013/000000 \r\n@",
            "<LF> on byte location opens next byte location"},
        {"10;/2;\r10/\rP",     "\n000000\n@10;/000000 2;\r\n@10/000002 \r\n@",
            "Semicolon character is ignored"},
        {"0/2/\rP",            "\n000000\n@0/000000 2/?\n@",
            "Address can not be opened on entering address value"},
        {"R0/2/\rP",           "\n000000\n@R0/000000 2/?\n@",
            "Address can not be opened on entering register value"},
        {"R0/R\rP",          "\n000000\n@R0/000000 R?\n@",
            "Register can not be opened on entering register value"},
        {"0/R\rP",           "\n000000\n@0/000000 R?\n@",
            "Register can not be opened on entering address value"}
    };

    for (TestSequence testSequence : testSequences)
        executeSequence (testSequence);
}


TEST_F (KDF11_A_ODTTest, BinaryDumpReturnsBytes)
{
    // Fill the memory locations 0402-0412 with ASCII characters to be able
    // to check the correct result of the binary dump, followed by a Binary
    // Dump command plus two characters forming the address to start the dump.
    // The last command is the Proceed command to exit the command
    // processor. The characters 1 and 2 should result in the address 0402
    // as start address of the dump.

    TestSequence testSequence
    {
        "402/60\r404/61\r406/62\r410/63\r412/64\r\023\001\002P",
        "\n000000\n@402/000000 60\r\n@404/000000 61\r\n@406/000000 62\r\n@410/000000 63\r\n@412/000000 64\r\n@\02301234@",
        "Binary Dump command dumps memory"
    };

    executeSequence (testSequence);
}

// This test comprises the commands implemented in just the KD11-NA (LSI-11)
// processors. On the KDF11-A these commands will return an error.
TEST_F (KDF11_A_ODTTest, KD11FOnlyCommands)
{
    vector<TestSequence> testSequences =
    {
        {"10/^\rP",                 "\n000000\n@10/000000 ^?\n@",
            "Up arrow (^) on open location returns error"},
        {"10/10^P",          "\n000000\n@10/000000 10^?\n@",
            "Error on Up arrow when entering new address value"},
        {"R0/^\rP",                 "\n000000\n@R0/000000 ^?\n@",
            "Up arrow on open register returns error"},
        {"RS/^\rP",                 "\n000000\n@RS/000000 ^?\n@",
            "Up arrow on opened PSW returns error"},
        {"R1/10^\rR1/\rP",          "\n000000\n@R1/000000 10^?\n@",
            "Error on Up arrow when entering new register value"},
        {"RS/10^RS/\rP",            "\n000000\n@RS/000000 10^?\n@",
            "Error on Up arrow when entering new PSW value"},
        {"10/@\rP",                 "\n000000\n@10/000000 @?\n@",
            "At sign returns error"},
        {"10/200@\rP",         "\n000000\n@10/000000 200@?\n@",
            "Error at at sign when entering new address value"},
        {"R0/@\rP",                 "\n000000\n@R0/000000 @?\n@",
            "At sign on register returns error"},
        {"RS/@\rP",                 "\n000000\n@RS/000000 @?\n@",
            "At sign on PSW return error"},
        {"1000/_\rP",               "\n000000\n@1000/000000 _?\n@",
            "Error at opening new address with back arrow"},
        {"1000/200_\rP",            "\n000000\n@1000/000000 200_?\n@",
            "Entering new address value with back arrow returns error"},
        {"R0/_P",                   "\n000000\n@R0/000000 _?\n@",
            "Error at closing register with back arrow"},
        {"4/4\010\rP",         "\n000000\n@4/000000 4?\n@",
            "Error on rub out of address digit can be rubbed out"},
        {"R0/123\010\rP", "\n000000\n@R0/000000 123?\n@",
            "Error on rub out of register value digit"},

        {"0/2/\rP",       "\n000000\n@0/000000 2/?\n@", 
            "Address location can be opened without closing first location"},
        {"MP",                          "\n000000\n@M?\n@",
            "Maintenance command returns error"}
    };

    for (TestSequence testSequence : testSequences)
        executeSequence (testSequence);
}
