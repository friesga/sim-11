#include "qbus/qbus.h"
#include "kd/kdf11_a/kdf11_a.h"

#include <gtest/gtest.h>

// Verify the PSW can be accessed via address 0177776. Write all one's to
// the PSW and expect all bits to be set except for the T-bit.
//
TEST (KDF11_A_PSWTEST, PswCanBeAccessedViaAddress)
{
    Qbus qbus;
    KDF11_A kdf11_a (&qbus);
    u16 psw;

    u16 const PswAddress = 0177776;

    kdf11_a.writeWord (PswAddress, 0177777);
    EXPECT_EQ (kdf11_a.read (PswAddress, &psw), StatusCode::OK);
    EXPECT_EQ (psw, 0177757);
}

// Verify that a MOV #0, @#PS instruction actually clears the PSW (and does
// not set the Z-bit). See JKDBD0 test 33.
TEST (KDF11_A_PSWTEST, MOVDoesNotSetCC)
{
    Qbus qbus;
    KDF11_A kdf11a (&qbus);
    KDF11_AInstruction instrDecoder;
    u16 const PswAddress = 0177776;

    // MOV #0, @#PS
    unique_ptr<LSI11Instruction> instruction {instrDecoder.decode (&kdf11a.cpu (), 0012737)};

    // Assume the MOV instruction is at address 0, so the second and third
    // word of the instruction are at address 2 and 4.
    // be at address 2. Address 8 = 100, R1 = 10
    kdf11a.cpu ().registers () [7] = 2;
    kdf11a.cpu ().putWord (2, 0);
    kdf11a.cpu ().putWord (4, 0177776);

    instruction->execute ();
    
    u16 psw;
    EXPECT_EQ (kdf11a.read (PswAddress, &psw), StatusCode::OK);
    EXPECT_EQ (psw, 0);
}