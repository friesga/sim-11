#include "interruptrequest/interruptrequest.h"
#include "threadsafecontainers/threadsafeprioqueue.h"

#include <gtest/gtest.h>

// Verify InterruptRequests have strict weak ordering
TEST (InterruptRequest, isOrdered)
{
    InterruptRequest const prio12_0 {TrapPriority::BusError, 0, 0, 004};
    InterruptRequest const prio12_1 {TrapPriority::BusError, 1, 0, 004};
    InterruptRequest const prio12_x {TrapPriority::BusError, 0, 0, 0};

    InterruptRequest const prio10_0 {TrapPriority::TraceTrap, 0, 0, 014};
    InterruptRequest const prio10_1 {TrapPriority::TraceTrap, 1, 0, 014};

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
    InterruptRequest const intrptReqA {TrapPriority::BusError, 0, 0, 004};
    InterruptRequest const intrptReqB {TrapPriority::BusError, 0, 0, 000};
    ThreadSafePrioQueue<InterruptRequest> queue;

    queue.push (intrptReqA);
    EXPECT_TRUE (queue.size() == 1);

    queue.erase (intrptReqB);
    EXPECT_TRUE (queue.size() == 0);
}

TEST (InterruptRequest, functionOrderisOrdered)
{
    InterruptRequest const order0 {TrapPriority::BusError, 0, 0, 004};
    InterruptRequest const order1 {TrapPriority::BusError, 0, 1, 004};
    
    EXPECT_TRUE (order0 < order1);
}