#include "dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "kd11/odt/kd11odt.h"
#include "testconsole.h"

using namespace KD11_F;

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

class KD11ODTTest : public ::testing::Test
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
        unique_ptr<Console> console = make_unique<TestConsole> ();

        // 'Steal' a raw pointer from the unique_ptr to be able to send and
        // receive characters to and from the TestConsole.
        TestConsole *testConsole = static_cast<TestConsole*> (console.get ());

        Qbus bus;
        KD11CPU kd11cpu (&bus);
        DLV11J dlv11j {&bus, std::move (console)};
        MSV11D msv11d (&bus);
        bus.installModule (0, &dlv11j);
        bus.installModule (1, &msv11d);

        // Send characters to the DLV11J
        for (char c : sequence.input)
            testConsole->send (c);

        // Let ODT process the input sequence
        KD11ODT kd11odt {&bus, kd11cpu};
        kd11odt.run ();

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
// 0173000.
//
TEST_F (KD11ODTTest, KDF11PlusKD11_FCommands)
{
    vector<TestSequence> testSequences =
    {
        {"P",                  "\n173000\n@P",
            "Exit with Proceed"},
        {"9P",                 "\n173000\n@9?\n@",
            "Error on invalid char at prompt"},
        {"19P",                "\n173000\n@19?\n@",
            "Error while entering address"},
        {"10\rP",              "\n173000\n@10\r\n?\n@",
            "<CR> while entering address"},
        {"10/\rP",             "\n173000\n@10/000000 \r\n@",
            "Address can be opened and closed"},
        {"10/2\r10/\rP",       "\n173000\n@10/000000 2\r\n@10/000002 \r\n@",
            "New address value can be entered"},
        {"10/9\rP",            "\n173000\n@10/000000 9?\n@",
            "Error on invalid char at open address"},
        {"10/19P",             "\n173000\n@10/000000 19?\n@",
            "Error on invalid char at new address value"},
        {"10/\n\rP",           "\n173000\n@10/000000 \n@000012/000000 \r\n@",
            "<LF> opens next location"},
        {"R0/\rP",             "\n173000\n@R0/000000 \r\n@",
            "R starts register entry"},
        {"$0/\rP",             "\n173000\n@$0/000000 \r\n@",
            "$ starts register entry"},
        {"RS/\rP",             "\n173000\n@RS/000000 \r\n@",
            "RS starts PSW entry"},
        {"R0/10\rR0/\rP",      "\n173000\n@R0/000000 10\r\n@R0/000010 \r\n@",
            "New register value can be entered"},
        {"RS/10\rRS/\rP",      "\n173000\n@RS/000000 10\r\n@RS/000010 \r\n@",
            "New PSW value can be entered"},
        {"RS/21\rRS/\rP",      "\n173000\n@RS/000000 21\r\n@RS/000001 \r\n@",
            "PSW T-bit cannot be changed"},
        {"R0/19P",             "\n173000\n@R0/000000 19?\n@",
            "Error on invalid char at new register value"},
        {"R76543210/\rP",      "\n173000\n@R76543210/000000 \r\n@",
            "Digits 0-7 accepted as register"},
        {"R01/23\rR1/\rP",     "\n173000\n@R01/000000 23\r\n@R1/000023 \r\n@",
            "Last digit entered determines register"},
        {"R1077/23\rRS/\rP",   "\n173000\n@R1077/000000 23\r\n@RS/000003 \r\n@",
            "Register 077 indicates PSW"},
        {"R1477/23\rRS/\rP",   "\n173000\n@R1477/000000 23\r\n@RS/000003 \r\n@",
            "Register 477 indicates PSW"},
        {"R9\rP",              "\n173000\n@R9?\n@",
            "Error on invalid char at register entry"},
        {"R019\rP",            "\n173000\n@R019?\n@",
            "Error on invalid char while entering register"},
        {"R0/177777\rR0/\rP",  "\n173000\n@R0/000000 177777\r\n@R0/177777 \r\n@",
            "New register value can be entered"},
        {"R0/200000\rP",       "\n173000\n@R0/000000 200000\r\n?\n@",
            "Error on invalid value at new register value"},
        {"R7/\n\rP",           "\n173000\n@R7/173000 \n@R0/000000 \r\n@",
            "<LF> opens next register and rolls over to R0"},
        {"RS/\n\rP",           "\n173000\n@RS/000000 \n@",
            "<LF> on opened PSW closes location"},
        {"R0/10\n\rR0/\rP",    "\n173000\n@R0/000000 10\n@R1/000000 \r\n@R0/000010 \r\n@",
            "New register value can be entered with <LF>"},
        {"0/10\n\r0/\rP",      "\n173000\n@0/000000 10\n@000002/000000 \r\n@0/000010 \r\n@",
            "New address value can be entered with <LF>"},
        {"RS/10\nRS/\rP",      "\n173000\n@RS/000000 10\n@RS/000010 \r\n@",
            "New PSW value can be entered with <LF>"},
        {"10/1012345\r10/\rP", "\n173000\n@10/000000 1012345\r\n@10/012345 \r\n@",
            "Last 6 digits determine new address value"},
        {"R0/1012345\rR0/\rP", "\n173000\n@R0/000000 1012345\r\n@R0/012345 \r\n@",
            "Last 6 digits determine new register value"},
        {"100000010/2\r10/\rP","\n173000\n@100000010/000000 2\r\n@10/000002 \r\n@",
            "Last 8 digits determine address"},
        {"200000/P",           "\n173000\n@200000/?\n@",
            "Error on invalid address"},
        {"0/177777\r1/\rP",  "\n173000\n@0/000000 177777\r\n@1/177777 \r\n@",
            "Clear least significant bit on odd address"},
        {"/P",               "\n173000\n@/?\n@",
            "Error on / with no location open"},
        {"10/2\r/\rP",       "\n173000\n@10/000000 2\r\n@/000002 \r\n@",
            "Address location can be reopened"},
        {"R0/2\r/\rP",       "\n173000\n@R0/000000 2\r\n@/000002 \r\n@",
            "Register location can be reopened"},
        {"RS/2\r/\rP",       "\n173000\n@RS/000000 2\r\n@/000002 \r\n@",
            "PSW location can be reopened"},
        {"GP",               "\n173000\n@G?\n@",
            "Error on Go command without address"},
        {"200G",             "\n173000\n@200G",
            "Go command with address accepted"},
        {"177774/P",         "\n173000\n@177774/?\n@",
            "Error on opening invalid address"},
        {"0/2\r177774//\rP", "\n173000\n@0/000000 2\r\n@177774/?\n@/000002 \r\n@",
            "Last opened location not set on opening invalid address"},
        {"157776/\nP",       "\n173000\n@157776/000000 \n@160000/?\n@",
            "Error on next opened invalid address"}
    };

    for (TestSequence testSequence : testSequences)
        executeSequence (testSequence);
}


TEST_F (KD11ODTTest, BinaryDumpReturnsBytes)
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
        "\n173000\n@402/000000 60\r\n@404/000000 61\r\n@406/000000 62\r\n@410/000000 63\r\n@412/000000 64\r\n@\02301234@",
        "Binary Dump command dumps memory"
    };

    executeSequence (testSequence);
}

// This test comprises the commands implemented in just the KD11-F (LSI-11)
// processors
TEST_F (KD11ODTTest, KD11FOnlyCommands)
{
    vector<TestSequence> testSequences =
    {
        {"10/]\rP",                 "\n173000\n@10/000000 ]\n@000006/000000 \r\n@",
            "Up arrow (]) opens previous location"},
        {"10/10]\r10/\rP",          "\n173000\n@10/000000 10]\n@000006/000000 \r\n@10/000010 \r\n@",
            "New address value can be entered with up arrow"},
        {"R0/]\rP",                 "\n173000\n@R0/000000 ]\n@R7/173000 \r\n@",
            "up arrow opens next register and rolls over to R7"},
        {"RS/]\rP",                 "\n173000\n@RS/000000 ]\n@",
            "up arrow on opened PSW closes location"},
        {"R1/10]\rR1/\rP",          "\n173000\n@R1/000000 10]\n@R0/000000 \r\n@R1/000010 \r\n@",
            "New register value can be entered with up arrow"},
        {"RS/10]RS/\rP",            "\n173000\n@RS/000000 10]\n@RS/000010 \r\n@",
            "New PSW value can be entered with uparrow"},
        {"10/@\rP",                 "\n173000\n@10/000000 @\n@000000/000000 \r\n@",
            "At sign opens indirect address"},
        {"10/200@\r10/\rP",         "\n173000\n@10/000000 200@\n@000200/000000 \r\n@10/000200 \r\n@",
            "New address value can be entered with at sign"},
        {"R0/@\rP",                 "\n173000\n@R0/000000 @\n@000000/000000 \r\n@",
            "At sign on register opens indirect address"},
        {"R0/200@\rR0/\rP",         "\n173000\n@R0/000000 200@\n@000200/000000 \r\n@R0/000200 \r\n@",
            "New register value can be entered with at sign"},
        {"RS/@\rP",                 "\n173000\n@RS/000000 @\n@000000/000000 \r\n@",
            "At sign on PSW opens indirect address"},
        {"200/\rRS/4@\rRS/\rP",     "\n173000\n@200/000000 \r\n@RS/000000 4@\n@000200/000000 \r\n@RS/000004 \r\n@",
            "New address value can be entered with at sign and opens last address location"},
        {"R0/\rRS/4@\rRS/\rP",      "\n173000\n@R0/000000 \r\n@RS/000000 4@\n@000000/000000 \r\n@RS/000004 \r\n@",
            "New register value can be entered with at sign and opens last address location"},
        {"RS/4@RS/\rP",             "\n173000\n@RS/000000 4@\n@/?\n@RS/000004 \r\n@",
            "New PSW value can be entered with at sign and returns error in case no location is open"},
        {"100/100\r200/100@\r/\rP", "\n173000\n@100/000000 100\r\n@200/000000 100@\n@000100/000100 \r\n@/000100 \r\n@",
            "New address value can be entered with at sign and sets last address location"},
        {"100/100\rR0/100@\r/\rP",  "\n173000\n@100/000000 100\r\n@R0/000000 100@\n@000100/000100 \r\n@/000100 \r\n@",
            "New register value can be entered with at sign and sets last address location"},
        {"100/100\rRS/4@\r/\rP",    "\n173000\n@100/000000 100\r\n@RS/000000 4@\n@000100/000100 \r\n@/000100 \r\n@",
            "New register value can be entered with at sign and keeps last address location"},
        {"1000/_\rP",               "\n173000\n@1000/000000 _\n@001002/000000 \r\n@",
            "New address can be opened with back arrow"},
        {"1000/200_\rP",            "\n173000\n@1000/000000 200_\n@001202/000000 \r\n@",
            "New address value can be entered with back arrow"},
        {"R0/_P",                   "\n173000\n@R0/000000 _\n@",
            "Register can be closed with back arrow"},
        {"R0/200_R0/\rP",           "\n173000\n@R0/000000 200_\n@R0/000200 \r\n@",
            "Register value can be entered with back arrow"},
        {"4/4\r2\0104/\rP",         "\n173000\n@4/000000 4\r\n@2\\4/000004 \r\n@",
            "Address digit can be rubbed out"},
        {"2\010\010/\rP",           "\n173000\n@2\\\\/?\n@",
            "Error if more address digits rubbed out then available"},
        {"0/177777\r0/123\010\r0/\rP", "\n173000\n@0/000000 177777\r\n@0/177777 123\\\r\n@0/000012 \r\n@",
            "Address value digit can be rubbed out"},
        {"0/177777\r0/123\010\010\010\r0/\rP", "\n173000\n@0/000000 177777\r\n@0/177777 123\\\\\\\r\n@0/000000 \r\n@",
            "New address value is zero if all digits erased"},
        {"0/177777\r0/\010\r0/\rP", "\n173000\n@0/000000 177777\r\n@0/177777 \\\r\n@0/000000 \r\n@",
            "New value is zero if no digit available"},
        {"0/177777\r0/123\010\010\010\010\r0/\rP", "\n173000\n@0/000000 177777\r\n@0/177777 123\\\\\\\\\r\n@0/000000 \r\n@",
            "New address value is zero if more digits erased than available"},
        {"R2\0104/177777\r4/\rP",   "\n173000\n@R2\\4/000000 177777\r\n@4/177777 \r\n@",
            "Rubout of register number enters address mode (1)"},
        {"R12\0104/177777\r4/\rP",  "\n173000\n@R12\\4/000000 177777\r\n@4/177777 \r\n@",
            "Rubout of register number enters address mode (2)"},
        {"R2\010/P",                "\n173000\n@R2\\/?\n@",
            "Error on open address if no address entered after rubout of register number"},
        {"R0/177777\rR0/123\010\rR0/\rP", "\n173000\n@R0/000000 177777\r\n@R0/177777 123\\\r\n@R0/000012 \r\n@",
            "Register value digit can be rubbed out"},
        {"R0/177777\rR0/123\010\010\010\rR0/\rP", "\n173000\n@R0/000000 177777\r\n@R0/177777 123\\\\\\\r\n@R0/000000 \r\n@",
            "New register value is zero if all digits erased"},
        {"R0/177777\rR0/\010\rR0/\rP", "\n173000\n@R0/000000 177777\r\n@R0/177777 \\\r\n@R0/000000 \r\n@",
            "New register value is zero if no digit available"},
        {"R0/177777\rR0/123\010\010\010\010\rR0/\rP", "\n173000\n@R0/000000 177777\r\n@R0/177777 123\\\\\\\\\r\n@R0/000000 \r\n@",
            "New register value is zero if more digits erased than available"},
        {"2/177777\r0/2/\rP",       "\n173000\n@2/000000 177777\r\n@0/000000 2/177777 \r\n@", 
            "Address location can be opened without closing first location"},
        {"p",                           "\n173000\n@p",
            "Lowercase proceed command accepted"},
        {"r0/\rP",                      "\n173000\n@r0/000000 \r\n@",
            "Lowercase register designator accepted"},
        {"rs/\rP",                      "\n173000\n@rs/000000 \r\n@",
            "Lowercase PSW designator accepted"},
        {"200g",                        "\n173000\n@200g",
            "Lowercase Go command accepted"}
    };

    for (TestSequence testSequence : testSequences)
        executeSequence (testSequence);
}
