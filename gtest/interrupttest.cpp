#include "bus/unibus/unibus.h"

#include <gtest/gtest.h>

TEST (InterruptTest, interruptsAreAcknowledged)
{
    bool interruptAcknowledged {false};
    Unibus unibus;
    InterruptRequest ir;

    unibus.setInterrupt (TrapPriority::BusError, 0, 0, 0,
        [&] () { interruptAcknowledged = true; });
    unibus.getIntrptReq (ir);

    EXPECT_TRUE (interruptAcknowledged);
}