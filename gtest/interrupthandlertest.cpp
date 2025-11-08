#include "bus/interrupthandler/interrupthandler.h"

#include <gtest/gtest.h>

TEST (InterruptHandlerTest, intrpReqQueueisEmpty)
{
    InterruptHandler interruptHandler {};
    EXPECT_FALSE (interruptHandler.intrptReqAvailable ());
}

TEST (InterruptHandlerTest, intrptCanBeSet)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (TrapPriority::BusError, 0, 0, 004);
    EXPECT_TRUE (interruptHandler.containsInterrupt (TrapPriority::BusError, 0, 0));
}

TEST (InterruptHandlerTest, intrptCanBeCleared)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (TrapPriority::BusError, 0, 0, 004);
    EXPECT_TRUE (interruptHandler.containsInterrupt (TrapPriority::BusError, 0, 0));

    interruptHandler.clearInterrupt (TrapPriority::BusError, 0, 0);
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::BusError, 0, 0));
}

TEST (InterruptHandlerTest, allIntrptsCanBeCleared)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (TrapPriority::BusError, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR4, 0, 0, 004);
    EXPECT_TRUE (interruptHandler.containsInterrupt (TrapPriority::BusError, 0, 0));
    EXPECT_TRUE (interruptHandler.containsInterrupt (TrapPriority::BR4, 0, 0));

    interruptHandler.clearInterrupts ();
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::BusError, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::BR4, 0, 0));
}

TEST (InterruptHandlerTest, intrptsArePriorityOrdered)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (TrapPriority::BusError, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR4, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR5, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR6, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR7, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::InstructionTrap, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::PowerFail, 0, 0, 004);

    EXPECT_EQ (interruptHandler.intrptPriority (),
        static_cast<u8> (TrapPriority::BusError));

    interruptHandler.clearInterrupt (TrapPriority::BusError, 0, 0);
    EXPECT_EQ (interruptHandler.intrptPriority (),
        static_cast<u8> (TrapPriority::InstructionTrap));
}

TEST (InterruptHandlerTest, correctIntrptFound)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (TrapPriority::BusError, 0, 0, 004);

    EXPECT_TRUE (interruptHandler.containsInterrupt (TrapPriority::BusError, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::BR4, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::BR5, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::BR6, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::BR7, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::InstructionTrap, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::PowerFail, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::TraceTrap, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::Event, 0, 0));

    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::BusError, 1, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (TrapPriority::BusError, 0, 1));
}

TEST (InterruptHandlerTest, intrptsRetrievedInPriorityOrder)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (TrapPriority::BusError, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR4, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR5, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR6, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR7, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::InstructionTrap, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::PowerFail, 0, 0, 004);

    InterruptRequest retrievedIntrpt {};

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BusError, 0, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::InstructionTrap, 0, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::PowerFail, 0, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR7, 0, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR6, 0, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR5, 0, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR4, 0, 0, 004));

    EXPECT_FALSE (interruptHandler.intrptReqAvailable ());
}

TEST (InterruptHandlerTest, intrptsRetrievedInBusOrder)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (TrapPriority::BR4, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR4, 1, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR4, 2, 0, 004);

    InterruptRequest retrievedIntrpt {};

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR4, 2, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR4, 1, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR4, 0, 0, 004));

    EXPECT_FALSE (interruptHandler.intrptReqAvailable ());
}

TEST (InterruptHandlerTest, intrptsRetrievedInFunctionOrder)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (TrapPriority::BR4, 0, 0, 004);
    interruptHandler.setInterrupt (TrapPriority::BR4, 0, 1, 004);
    interruptHandler.setInterrupt (TrapPriority::BR4, 0, 2, 004);

    InterruptRequest retrievedIntrpt {};

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR4, 0, 2, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR4, 0, 1, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (TrapPriority::BR4, 0, 0, 004));

    EXPECT_FALSE (interruptHandler.intrptReqAvailable ());
}