#include "kdf11_a_odt.h"

// This command is used for manufacturing test purposes and is not a normal
// user command. [...] The protocol is as follows:
// 1. After a prompt character, console OOT receives a control-shift-S
//    command and echoes it.
// 2. The host system at the other end of the serial line must send two
//    8-bit bytes which console OOT interprets as a starting address.
//    These two bytes are not echoed.
//    The first byte specifies starting address <15:8> and the second
//    byte specifies starting address <7:0>. Bus address bits <17:16>
//    are always forced to be 0; the dump command is restricted to the
//    first 64 KB of address space.
// 3. After the second address byte has been received, console OOT
//    outputs 12 octal bytes to the serial line starting at the address
//    previously specified. When the output is finished, console OOT
//    prints <CR><LF>@.
//
// (Microcomputer and Memories EB-20912-20)
//
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (AtPrompt_1 &&, BinaryDumpCmdEntered)
{
    // Expect two bytes and transform it into the starting address.
    // The two bytes forming the address have to be read in two separate
    // statements to make sure the high byte of the address is read first.
    u16 highByte = context_->console_->read () << 8;
    u16 startAddress = highByte | context_->console_->read ();

    // Dump 10 bytes i.e. 5 words as binaries
    for (size_t numWords = 0; numWords < 5; ++numWords)
    {
        context_->console_->write (context_->bus_->read (startAddress));
        startAddress += 2;
    }

    return AtPrompt_1 {};
}