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