#include "interruptrequest/interruptrequest.h"
#include "threadsafeprioqueue/threadsafeprioqueue.h"

#include <gtest/gtest.h>

// Verify InterruptRequests have strict weak ordering
TEST (InterruptRequest, isOrdered)
{
    InterruptRequest const prio12_0 {RequestType::Trap, TrapPriority::BusError, 0, 004};
    InterruptRequest const prio12_1 {RequestType::Trap, TrapPriority::BusError, 1, 004};
    InterruptRequest const prio12_x {RequestType::IntrptReq, TrapPriority::BusError, 0, 0};

    InterruptRequest const prio10_0 {RequestType::Trap, TrapPriority::TraceTrap, 0, 014};
    InterruptRequest const prio10_1 {RequestType::Trap, TrapPriority::TraceTrap, 1, 014};

    EXPECT_TRUE (prio10_0 < prio12_0);
    EXPECT_TRUE (prio10_0 < prio10_1);

    EXPECT_TRUE (prio12_0 == prio12_0);
    EXPECT_TRUE (prio12_0 == prio12_x);
}

// Verify an InterruptRequest can be pushed and erased from 
// a ThreadSafePrioQueue. Two InterruptRequests are equal if
// their TrapPriority and bus order are equal.
TEST (InterruptRequest, canBePushedAndCleared)
{
    InterruptRequest const intrptReqA {RequestType::Trap, 
        TrapPriority::BusError, 0, 004};
    InterruptRequest const intrptReqB {RequestType::IntrptReq, 
        TrapPriority::BusError, 0, 000};
    ThreadSafePrioQueue<InterruptRequest> queue;

    queue.push (intrptReqA);
    EXPECT_TRUE (queue.size() == 1);

    queue.erase (intrptReqB);
    EXPECT_TRUE (queue.size() == 0);
}