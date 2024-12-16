#include "signal/signal.h"

#include <gtest/gtest.h>
#include <vector>

using std::bind;
using std::placeholders::_1;
using std::cout;
using std::vector;

// Definition of the test fixture. This will render an initialzed testSignal
// at the start of each test
class SignalTest : public ::testing::Test
{
public:
    Signal initFalseSignal;
    Signal initTrueSignal {true};
};

class Subscriber
{
public:
    Subscriber (Signal &signal);
    void signalReceiver (bool signalValue);
    void sendSignalToOthers ();
    void sendSignalToAll ();
    void cycleSignal ();
    bool receivedSignalValues (vector<bool>);
 
private:
    Signal &signal_;
    Signal::SubscriberKey ourKey_;
    vector<bool> receivedSignalValues_;
};

// Constructor
// Subscribe to the bus signals we have to be informed of
inline Subscriber::Subscriber (Signal &signal)
    :
    signal_ {signal},
    ourKey_ {0},
    receivedSignalValues_ {}
{
    ourKey_ = signal_.subscribe (bind (&Subscriber::signalReceiver, this, _1));
}

inline void Subscriber::signalReceiver (bool signalValue)
{
    receivedSignalValues_.push_back (signalValue);
}

inline void Subscriber::sendSignalToOthers ()
{
    signal_.set (true, ourKey_);
}

inline void Subscriber::sendSignalToAll ()
{
    signal_.set (true);
}

inline void Subscriber::cycleSignal ()
{
    signal_.cycle ();
}

inline bool Subscriber::receivedSignalValues (vector<bool> expectedSignalValues)
{
    return receivedSignalValues_ == expectedSignalValues;
}

TEST_F (SignalTest, defaultValueIsSet)
{
    EXPECT_FALSE (static_cast<bool> (initFalseSignal));
    EXPECT_TRUE (static_cast<bool> (initTrueSignal));
}

TEST_F (SignalTest, signalCanBeSetAndTested)
{
    // Signals can be set and tested
    initFalseSignal.set (true);

    EXPECT_TRUE (static_cast<bool> (initFalseSignal));
}

TEST_F (SignalTest, signalCanBeSentToAll)
{
    Subscriber subscriberA {initFalseSignal};
    Subscriber subscriberB {initFalseSignal};

    subscriberA.sendSignalToAll ();

    // Verify all subscribers are notified with the correct signal value
    EXPECT_TRUE (subscriberA.receivedSignalValues (vector<bool> {true}));
    EXPECT_TRUE (subscriberB.receivedSignalValues (vector<bool> {true}));
}

TEST_F (SignalTest, signalCanBeSentToOthers)
{
    Subscriber subscriberA {initFalseSignal};
    Subscriber subscriberB {initFalseSignal};

    subscriberA.sendSignalToOthers ();

    // Verify the signal sender is not notified
    EXPECT_TRUE (subscriberA.receivedSignalValues (vector<bool> {}));
    EXPECT_TRUE (subscriberB.receivedSignalValues (vector<bool> {true}));
}

TEST_F (SignalTest, initFalseSignalCanBeCycled)
{
    Subscriber subscriberA {initFalseSignal};
    Subscriber subscriberB {initFalseSignal};

    subscriberA.cycleSignal ();

    EXPECT_TRUE (subscriberA.receivedSignalValues (vector<bool> {true, false}));
    EXPECT_TRUE (subscriberB.receivedSignalValues (vector<bool> {true, false}));
}

TEST_F (SignalTest, initTrueSignalCanBeCycled)
{
    Subscriber subscriberA {initTrueSignal};
    Subscriber subscriberB {initTrueSignal};

    subscriberA.cycleSignal ();

    EXPECT_TRUE (subscriberA.receivedSignalValues (vector<bool> {false, true}));
    EXPECT_TRUE (subscriberB.receivedSignalValues (vector<bool> {false, true}));
}

TEST_F (SignalTest, signalCanBeBlockedAndUnblocked)
{
    initFalseSignal.block ();
    initFalseSignal.set (true);

    // The value of the signal should not be changed
    EXPECT_FALSE (static_cast<bool> (initFalseSignal));

    initFalseSignal.unblock ();
    initFalseSignal.set (true);

    // The value of the signal should now be changed
    EXPECT_TRUE (static_cast<bool> (initFalseSignal));
}

TEST_F (SignalTest, noNotificationOnblockedSignal)
{
    Subscriber subscriberA {initFalseSignal};
    Subscriber subscriberB {initFalseSignal};

    initFalseSignal.block ();

    subscriberA.cycleSignal ();

    // Verify no subscribers are notified
    EXPECT_TRUE (subscriberA.receivedSignalValues (vector<bool> {}));
    EXPECT_TRUE (subscriberB.receivedSignalValues (vector<bool> {}));
}