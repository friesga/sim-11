#include "lsi11/lsi11.h"
#include "rlv12/rlv12.h"

#include <gtest/gtest.h>

// Verify a file can be attached to a unit
TEST (RLV12, attachable)
{
    // Create bus structure, an RLV12 device and install the device
    LSI11 lsi;
    RLV12 rlv12Device {};
    lsi.bus.installModule (1, &rlv12Device);

    // Verify attaching a non-existing file return the appropriate error
    ASSERT_EQ (rlv12Device.unit (0)->attach ("non-existingfile"), 
        StatusCode::OpenError);
}