#include "bus/interruptrequest/interruptrequest.h"
#include "threadsafecontainers/threadsafeprioqueue.h"

#include <gtest/gtest.h>

// Verify InterruptRequests have strict weak ordering
TEST (InterruptRequest, isOrdered)
{
    InterruptRequest const prio7_0 {InterruptPriority::BR7, 0, 0, 004};
    InterruptRequest const prio7_1 {InterruptPriority::BR7, 1, 0, 004};
    InterruptRequest const prio7_x {InterruptPriority::BR7, 0, 0, 0};

    InterruptRequest const prio6_0 {InterruptPriority::BR6, 0, 0, 014};
    InterruptRequest const prio6_1 {InterruptPriority::BR6, 1, 0, 014};

    EXPECT_TRUE (prio6_0 < prio7_0);
    EXPECT_TRUE (prio6_0 < prio6_1);

    EXPECT_TRUE (prio6_0 == prio6_0);
    EXPECT_TRUE (prio7_0 == prio7_x);
}

// Verify an InterruptRequest can be pushed and erased from 
// a ThreadSafePrioQueue. Two InterruptRequests are equal if
// their InterruptPriority and bus order are equal.
TEST (InterruptRequest, canBePushedAndCleared)
{
    InterruptRequest const intrptReqA {InterruptPriority::BR7, 0, 0, 004};
    InterruptRequest const intrptReqB {InterruptPriority::BR7, 0, 0, 000};
    ThreadSafePrioQueue<InterruptRequest> queue;

    queue.push (intrptReqA);
    EXPECT_TRUE (queue.size() == 1);

    queue.erase (intrptReqB);
    EXPECT_TRUE (queue.size() == 0);
}

TEST (InterruptRequest, functionOrderisOrdered)
{
    InterruptRequest const order0 {InterruptPriority::BR7, 0, 0, 004};
    InterruptRequest const order1 {InterruptPriority::BR7, 0, 1, 004};

    EXPECT_TRUE (order0 < order1);
}