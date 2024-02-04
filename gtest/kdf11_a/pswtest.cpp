#include "qbus/qbus.h"
#include "kd/kdf11/kdf11_a/kdf11_a.h"
#include "msv11d/msv11d.h"

#include <gtest/gtest.h>

class KDF11_A_PSWTEST : public ::testing::Test
{
protected:
    Qbus qbus;
    KDF11_A* kdf11a  = new KDF11_A (&qbus);
    MSV11D*  memory  = new MSV11D (&qbus);
    KDF11Instruction instrDecoder;
    u16 const PswAddress = 0177776;

    void SetUp() override
    {
        qbus.installModule (1, kdf11a);
        qbus.installModule (2, memory);

        kdf11a->start ();
    }
};

// Verify the PSW can be accessed via address 0177776. Write all one's to
// the PSW and expect all bits to be set except for the T-bit.
//
TEST_F (KDF11_A_PSWTEST, PswCanBeAccessedViaAddress)
{
    u16 psw;

    kdf11a->writeWord (PswAddress, 0177777);
    EXPECT_EQ (kdf11a->read (PswAddress, &psw), StatusCode::OK);
    EXPECT_EQ (psw, 0177757);
}


// Verify that a MOV #0, @#PS instruction actually clears the PSW (and does
// not set the Z-bit). See JKDBD0 test 33.
TEST_F (KDF11_A_PSWTEST, MOVDoesNotSetCC)
{
    kdf11a->writeWord (PswAddress, 0177777);

    // MOVB #0, @#PS
    // Assume the MOV instruction is at address 0, so the second and third
    // word of the instruction are at address 2 and 4.
    kdf11a->cpuData ()->registers () [7] = 2;
    kdf11a->mmu ()->putWord (2, 0);
    kdf11a->mmu ()->putWord (4, 0177776);
    unique_ptr<LSI11Instruction> instruction {instrDecoder.decode (kdf11a->cpuData (),
        kdf11a->cpuControl (), kdf11a->mmu (), 0012737)};

    EXPECT_TRUE (instruction->execute ());
    
    u16 psw;
    EXPECT_EQ (kdf11a->read (PswAddress, &psw), StatusCode::OK);
    EXPECT_EQ (psw, 0);
}


// Verify that a MOVB #0, @#PS instruction actually clears the lower byte 
// of the PSW (and does not set the Z-bit).
TEST_F (KDF11_A_PSWTEST, MOVBDoesNotSetCC)
{
    kdf11a->writeWord (PswAddress, 0177777);

    // MOVB #0, @#PS
    // Assume the MOV instruction is at address 0, so the second and third
    // word of the instruction are at address 2 and 4.
    kdf11a->cpuData ()->registers () [7] = 2;
    kdf11a->mmu ()->putWord (2, 0);
    kdf11a->mmu ()->putWord (4, 0177776);
    unique_ptr<LSI11Instruction> instruction {instrDecoder.decode (kdf11a->cpuData (),
        kdf11a->cpuControl (), kdf11a->mmu (), 0112737)};

    EXPECT_TRUE (instruction->execute ());
    
    u16 psw;
    EXPECT_EQ (kdf11a->read (PswAddress, &psw), StatusCode::OK);
    EXPECT_EQ (psw, 0177400);
}

// Verify that a CLR @#PS instruction actually clears the PSW (and does
// not set the Z-bit). See JKDBD0 test 243.
TEST_F (KDF11_A_PSWTEST, CLRDoesNotSetCC)
{
    kdf11a->writeWord (PswAddress, 0177777);

    // CLR @#PS
    // Assume the CLR instruction is at address 0, so the second word of the
    // instruction is at address 2.
    kdf11a->cpuData ()->registers () [7] = 2;
    kdf11a->mmu ()->putWord (2, 0177776);
    unique_ptr<LSI11Instruction> instruction {instrDecoder.decode (kdf11a->cpuData (),
        kdf11a->cpuControl (), kdf11a->mmu (), 0005037)};

    EXPECT_TRUE (instruction->execute ());
    
    u16 psw;
    EXPECT_EQ (kdf11a->read (PswAddress, &psw), StatusCode::OK);
    EXPECT_EQ (psw, 0);
}

// Verify that a CLRB @#PS instruction actually clears the PSW (and does
// not set the Z-bit).
TEST_F (KDF11_A_PSWTEST, CLRBDoesNotSetCC)
{
    kdf11a->writeWord (PswAddress, 0177777);

    // CLR @#PS
    // Assume the CLR instruction is at address 0, so the second word of the
    // instruction is at address 2.
    kdf11a->cpuData ()->registers () [7] = 2;
    kdf11a->mmu ()->putWord (2, 0177776);
    unique_ptr<LSI11Instruction> instruction {instrDecoder.decode (kdf11a->cpuData (),
        kdf11a->cpuControl (), kdf11a->mmu (), 0105037)};

    EXPECT_TRUE (instruction->execute ());
    
    u16 psw;
    EXPECT_EQ (kdf11a->read (PswAddress, &psw), StatusCode::OK);
    EXPECT_EQ (psw, 0177400);
}
