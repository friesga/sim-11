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
    interruptHandler.setInterrupt (InterruptPriority::BR7, 0, 0, 004);
    EXPECT_TRUE (interruptHandler.containsInterrupt (InterruptPriority::BR7, 0, 0));
}

TEST (InterruptHandlerTest, intrptCanBeCleared)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (InterruptPriority::BR7, 0, 0, 004);
    EXPECT_TRUE (interruptHandler.containsInterrupt (InterruptPriority::BR7, 0, 0));

    interruptHandler.clearInterrupt (InterruptPriority::BR7, 0, 0);
    EXPECT_FALSE (interruptHandler.containsInterrupt (InterruptPriority::BR7, 0, 0));
}

TEST (InterruptHandlerTest, nonExistentIntrptCanBeCleared)
{
    InterruptHandler interruptHandler {};

    interruptHandler.clearInterrupt (InterruptPriority::BR7, 0, 0);
}

TEST (InterruptHandlerTest, allIntrptsCanBeCleared)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (InterruptPriority::BR7, 0, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR4, 0, 0, 004);
    EXPECT_TRUE (interruptHandler.containsInterrupt (InterruptPriority::BR7, 0, 0));
    EXPECT_TRUE (interruptHandler.containsInterrupt (InterruptPriority::BR4, 0, 0));

    interruptHandler.clearInterrupts ();
    EXPECT_FALSE (interruptHandler.containsInterrupt (InterruptPriority::BR7, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (InterruptPriority::BR4, 0, 0));
}

TEST (InterruptHandlerTest, intrptsArePriorityOrdered)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (InterruptPriority::BR7, 0, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR4, 0, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR5, 0, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR6, 0, 0, 004);

    EXPECT_EQ (interruptHandler.intrptPriority (),
        static_cast<u8> (InterruptPriority::BR7));

    interruptHandler.clearInterrupt (InterruptPriority::BR7, 0, 0);
    EXPECT_EQ (interruptHandler.intrptPriority (),
        static_cast<u8> (InterruptPriority::BR6));
}

TEST (InterruptHandlerTest, correctIntrptFound)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (InterruptPriority::BR7, 0, 0, 004);

    EXPECT_TRUE (interruptHandler.containsInterrupt (InterruptPriority::BR7, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (InterruptPriority::BR4, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (InterruptPriority::BR5, 0, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (InterruptPriority::BR6, 0, 0));

    EXPECT_FALSE (interruptHandler.containsInterrupt (InterruptPriority::BR7, 1, 0));
    EXPECT_FALSE (interruptHandler.containsInterrupt (InterruptPriority::BR7, 0, 1));
}

TEST (InterruptHandlerTest, intrptsRetrievedInPriorityOrder)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (InterruptPriority::BR4, 0, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR5, 0, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR6, 0, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR7, 0, 0, 004);

    InterruptRequest retrievedIntrpt {};

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR7, 0, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR6, 0, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR5, 0, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR4, 0, 0, 004));

    EXPECT_FALSE (interruptHandler.intrptReqAvailable ());
}

TEST (InterruptHandlerTest, intrptsRetrievedInBusOrder)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (InterruptPriority::BR4, 0, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR4, 1, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR4, 2, 0, 004);

    InterruptRequest retrievedIntrpt {};

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR4, 2, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR4, 1, 0, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR4, 0, 0, 004));

    EXPECT_FALSE (interruptHandler.intrptReqAvailable ());
}

TEST (InterruptHandlerTest, intrptsRetrievedInFunctionOrder)
{
    InterruptHandler interruptHandler {};
    interruptHandler.setInterrupt (InterruptPriority::BR4, 0, 0, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR4, 0, 1, 004);
    interruptHandler.setInterrupt (InterruptPriority::BR4, 0, 2, 004);

    InterruptRequest retrievedIntrpt {};

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR4, 0, 2, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR4, 0, 1, 004));

    EXPECT_TRUE (interruptHandler.getIntrptReq (retrievedIntrpt));
    EXPECT_EQ (retrievedIntrpt, InterruptRequest (InterruptPriority::BR4, 0, 0, 004));

    EXPECT_FALSE (interruptHandler.intrptReqAvailable ());
}
