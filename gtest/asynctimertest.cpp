#include "asynctimer/asynctimer.h"

#include <chrono>
#include <thread>
#include <gtest/gtest.h>

using namespace std;

atomic<int> timer1Elapsed = 0;
atomic<int> timer2Elapsed = 0;
atomic<int> timer3Elapsed = 0;

void onTimer1Elapsed (int i) { timer1Elapsed = i; }
void onTimer2Elapsed (int i) { timer2Elapsed = i; }
void onTimer3Elapsed (int i) { timer3Elapsed = i; }


// Verify that the specified function is executed
TEST (AsyncTimer, funcIsExecuted)
{
    AsyncTimer<int> timer {};
    int const timerId = 1;

    // Start the timer and verify it is running
    timer1Elapsed = 0;
    timer.start (onTimer1Elapsed, timerId, 500ms);
    EXPECT_TRUE (timer.isRunning (timerId));

    // Wait till the timer has finished, then verify the timer
    // is no longer running and the specified function has been executed.
    this_thread::yield();
    this_thread::sleep_for (chrono::seconds(1));
    EXPECT_FALSE (timer.isRunning (timerId));
    EXPECT_TRUE (timer1Elapsed == timerId);
}

// Verify a running timer can be cancelled
TEST (AsyncTimer, canBeCancelled)
{
    AsyncTimer<int> timer {};
    int const timerId = 1;

    // Start a long running timer and verify it is running
    timer1Elapsed = 0;
    timer.start (onTimer1Elapsed, timerId, 10'000ms);
    EXPECT_TRUE (timer.isRunning (timerId));

    // Cancel the timer and put this thread to sleep to give the started
    // thread the opportunity to perform the timer cancellation.
    timer.cancel(timerId);
    this_thread::sleep_for (chrono::seconds(1));

    EXPECT_FALSE (timer.isRunning (timerId));
    EXPECT_TRUE (timer1Elapsed == 0);
}

// Verify that after a timer has been cancelled a timer with the same
// id can be restarted
TEST (AsyncTimer, timerCanBeRestarted)
{
    AsyncTimer<int> timer {};
    int const timerId = 1;

    // Start a long running timer and verify it is running
    timer1Elapsed = 0;
    timer.start (onTimer1Elapsed, timerId, 10'000ms);
    EXPECT_TRUE (timer.isRunning (timerId));

    // Cancel the timer and put this thread to sleep to give the started
    // thread the opportunity to perform the timer cancellation.
    timer.cancel (timerId);
    this_thread::sleep_for (chrono::seconds(1));

    EXPECT_FALSE (timer.isRunning (timerId));
    EXPECT_TRUE (timer1Elapsed == 0);

    // Now restart the timer
    timer.start (onTimer1Elapsed, timerId, 10'000ms);

    // Allow the handler to queue the timer
    this_thread::sleep_for (chrono::seconds(1));

    // The timer should be running
    EXPECT_TRUE (timer.isRunning (timerId));
}

TEST (AsyncTimer, multipleTimersCanBeStarted)
{
    AsyncTimer<int> timer {};

    int const timerId1 = 1;
    int const timerId2 = 2;
    int const timerId3 = 3;
    
   // Start the timers and verify they are running
    timer1Elapsed = 0;
    timer2Elapsed = 0;
    timer3Elapsed = 0;

    timer.start (onTimer1Elapsed, timerId1, 500ms);
    timer.start (onTimer2Elapsed, timerId2, 600ms);
    timer.start (onTimer3Elapsed, timerId3, 700ms);

    EXPECT_TRUE (timer.isRunning (timerId1));
    EXPECT_TRUE (timer.isRunning (timerId2));
    EXPECT_TRUE (timer.isRunning (timerId3));

    // Wait till the timers are finished, then verify the timers
    // is no longer running and the specified functions have been executed.
    this_thread::yield();
    this_thread::sleep_for (chrono::seconds(1));

    EXPECT_FALSE (timer.isRunning(timerId1));
    EXPECT_FALSE (timer.isRunning(timerId2));
    EXPECT_FALSE (timer.isRunning(timerId3));

    EXPECT_TRUE (timer1Elapsed == 1);
    EXPECT_TRUE (timer2Elapsed == 2);
    EXPECT_TRUE (timer3Elapsed == 3);
}

TEST (AsyncTimer, noSuchTimer)
{
    AsyncTimer<int> timer;

    EXPECT_FALSE (timer.isRunning (0));
}