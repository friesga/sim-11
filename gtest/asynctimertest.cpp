#include "asynctimer/asynctimer.h"

#include <chrono>
#include <thread>
#include <gtest/gtest.h>

using namespace std;

struct Handler
{
    bool timerElapsed = false;

    void operator() () {timerElapsed = true; };
};

// Verify that the specified function is executed
TEST (AsyncTimer, funcIsExecuted)
{
    AsyncTimer<Handler> timer {};
    Handler handler;

    // Start the timer and verify it is running
    timer.start (&handler, 500ms);
    EXPECT_TRUE (timer.isRunning (&handler));

    // Wait till the timer has finished, then verify the timer
    // is no longer running and the specified function has been executed.
    this_thread::yield();
    this_thread::sleep_for (chrono::seconds(1));
    EXPECT_FALSE (timer.isRunning (&handler));
    EXPECT_TRUE (handler.timerElapsed);
}

// Verify a running timer can be cancelled
TEST (AsyncTimer, canBeCancelled)
{
    AsyncTimer<Handler> timer {};
    Handler handler;

    // Start a long running timer and verify it is running
    timer.start (&handler, 10'000ms);
    EXPECT_TRUE (timer.isRunning (&handler));

    // Cancel the timer and put this thread to sleep to give the started
    // thread the opportunity to perform the timer cancellation.
    timer.cancel (&handler);
    this_thread::sleep_for (chrono::seconds(1));

    EXPECT_FALSE (timer.isRunning (&handler));
    EXPECT_FALSE (handler.timerElapsed);
}

// Verify that after a timer has been cancelled a timer with the same
// id can be restarted
TEST (AsyncTimer, timerCanBeRestarted)
{
    AsyncTimer<Handler> timer {};
    Handler handler;

    // Start a long running timer and verify it is running
    timer.start (&handler, 10'000ms);
    EXPECT_TRUE (timer.isRunning (&handler));

    // Cancel the timer and put this thread to sleep to give the started
    // thread the opportunity to perform the timer cancellation.
    timer.cancel (&handler);
    this_thread::sleep_for (chrono::seconds(1));

    EXPECT_FALSE (timer.isRunning (&handler));
    EXPECT_FALSE (handler.timerElapsed);

    // Now restart the timer
    timer.start (&handler, 10'000ms);

    // Allow the handler to queue the timer
    this_thread::sleep_for (chrono::seconds(1));

    // The timer should be running
    EXPECT_TRUE (timer.isRunning (&handler));
}

TEST (AsyncTimer, multipleTimersCanBeStarted)
{
    AsyncTimer<Handler> timer {};
    Handler handler1;
    Handler handler2;
    Handler handler3;

       
    // Start the timers and verify they are running
    timer.start (&handler1, 500ms);
    timer.start (&handler2, 600ms);
    timer.start (&handler3, 700ms);

    EXPECT_TRUE (timer.isRunning (&handler1));
    EXPECT_TRUE (timer.isRunning (&handler2));
    EXPECT_TRUE (timer.isRunning (&handler3));

    // Wait till the timers are finished, then verify the timers
    // is no longer running and the specified functions have been executed.
    this_thread::yield();
    this_thread::sleep_for (chrono::seconds(1));

    EXPECT_FALSE (timer.isRunning(&handler1));
    EXPECT_FALSE (timer.isRunning(&handler2));
    EXPECT_FALSE (timer.isRunning(&handler3));

    EXPECT_TRUE (handler1.timerElapsed);
    EXPECT_TRUE (handler2.timerElapsed);
    EXPECT_TRUE (handler3.timerElapsed);
}

TEST (AsyncTimer, noSuchTimer)
{
    AsyncTimer<Handler> timer;

    EXPECT_FALSE (timer.isRunning (nullptr));
}