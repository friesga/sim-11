#include "asynctimer/asynctimer.h"

#include <chrono>
#include <thread>
#include <gtest/gtest.h>

using namespace std;

atomic<bool> timer1Elapsed = false;
atomic<bool> timer2Elapsed = false;
atomic<bool> timer3Elapsed = false;

void onTimer1Elapsed () { timer1Elapsed = true; }
void onTimer2Elapsed () { timer2Elapsed = true; }
void onTimer3Elapsed () { timer3Elapsed = true; }


// Verify that the specified function is executed
TEST (AsyncTimerTest, funcIsExecuted)
{
    AsyncTimer timer {};
    int timerId;

    // Start the timer and verify it is running
    timer1Elapsed = false;
    timer.start (onTimer1Elapsed, 500ms, &timerId);
    EXPECT_TRUE (timer.isRunning (&timerId));

    // Wait till the timer has finished, then verify the timer
    // is no longer running and the specified function has been executed.
    this_thread::yield();
    this_thread::sleep_for (chrono::seconds(1));
    EXPECT_FALSE (timer.isRunning(&timerId));
    EXPECT_TRUE (timer1Elapsed);
}

// Verify a running timer can be cancelled
TEST (AsyncTimerTest, canBeCancelled)
{
    AsyncTimer timer {};
    int timerId;

    // Start a long running timer and verify it is running
    timer1Elapsed = false;
    timer.start (onTimer1Elapsed, 10'000ms, &timerId);
    EXPECT_TRUE (timer.isRunning (&timerId));

    // Cancel the timer and put this thread to sleep to give the started
    // thread the opportunity to perform the timer cancellation.
    timer.cancel(&timerId);
    this_thread::sleep_for (chrono::seconds(1));

    // EXPECT_FALSE (timer.isRunning());
    EXPECT_FALSE (timer1Elapsed);
}

// Verify that after a timer has been cancelled a timer with the same
// id can be restarted
TEST (AsyncTimerTest, timerCanBeRestarted)
{
    AsyncTimer timer {};
    int timerId;

    // Start a long running timer and verify it is running
    timer1Elapsed = false;
    timer.start (onTimer1Elapsed, 10'000ms, &timerId);
    EXPECT_TRUE (timer.isRunning (&timerId));

    // Cancel the timer and put this thread to sleep to give the started
    // thread the opportunity to perform the timer cancellation.
    timer.cancel(&timerId);
    this_thread::sleep_for (chrono::seconds(1));

    EXPECT_FALSE (timer.isRunning(&timerId));
    EXPECT_FALSE (timer1Elapsed);

    // Now restart the timer
    timer1Elapsed = false;
    timer.start (onTimer1Elapsed, 10'000ms, &timerId);

    // Allow the handler to queue the timer
    this_thread::sleep_for (chrono::seconds(1));

    // The timer should be running
    EXPECT_TRUE (timer.isRunning (&timerId));
    EXPECT_FALSE (timer1Elapsed);
}

TEST (AsyncTimerTest, multipleTimersCanBeStarted)
{
    AsyncTimer timer {};
    
   // Start the timers and verify they are running
    timer1Elapsed = false;
    timer2Elapsed = false;
    timer3Elapsed = false;

    timer.start (onTimer1Elapsed, 500ms, &timer1Elapsed);
    timer.start (onTimer2Elapsed, 600ms, &timer2Elapsed);
    timer.start (onTimer3Elapsed, 700ms, &timer3Elapsed);

    EXPECT_TRUE (timer.isRunning (&timer1Elapsed));
    EXPECT_TRUE (timer.isRunning (&timer2Elapsed));
    EXPECT_TRUE (timer.isRunning (&timer3Elapsed));

    // Wait till the timers are finished, then verify the timers
    // is no longer running and the specified functions have been executed.
    this_thread::yield();
    this_thread::sleep_for (chrono::seconds(1));

    EXPECT_FALSE (timer.isRunning(&timer1Elapsed));
    EXPECT_FALSE (timer.isRunning(&timer2Elapsed));
    EXPECT_FALSE (timer.isRunning(&timer3Elapsed));

    EXPECT_TRUE (timer1Elapsed);
    EXPECT_TRUE (timer2Elapsed);
    EXPECT_TRUE (timer3Elapsed);
}

TEST (AsyncTimerTest, noSuchTimer)
{
    AsyncTimer timer;

    EXPECT_FALSE (timer.isRunning (nullptr));
}