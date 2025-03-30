
#include <gtest/gtest.h>

// A custom main() function for GoogleTest in which we can perform custom
// actions, such as enabling the tracing facility.
//
int main (int argc, char** argv)
{
    // The testing::InitGoogleTest() function parses the command line for
    // GoogleTest flags, and removes all recognized flags. This function
    // must be called before calling RUN_ALL_TESTS(), or the flags won’t
    // be properly initialized.
    testing::InitGoogleTest (&argc, argv);

    // The main function must return the value of RUN_ALL_TESTS ().
    return RUN_ALL_TESTS ();
}