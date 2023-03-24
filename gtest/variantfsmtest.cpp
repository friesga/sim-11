#include "variantfsm/fsm.h"

using variantFsm::TagType;

#include <gtest/gtest.h>

// Unit tests for the variantfsm state machine.
// 
// Definition of the fixture for these tests.

// Definition of the states
struct Off {};
struct On {};

// Definition of the events
struct SwitchOn {};
struct SwitchOff {};

// Definition of the events and states to use. The first state member of the
// variant is the initial state. The type std::monostate is included to be
// able to specify that no state transition has to be taken. In that case
// transition has to return std::monostate. The state machine remains in the
// same state and no exit and entry actions are executed.
// 
// Note: std::monostate should not be the first state in the variant as than
// the entry action of the state Off would not be executed and some tests
// would fail.
using State = std::variant<Off, On, std::monostate>;
using Event = std::variant<SwitchOff, SwitchOn>;

// The variable stateOffEntered is defined as a global variable
// to be able to verify it is set by VariantFsmTest::entry (Off).
// This is to verify the entry action of the initial state is
// executed on creation of the state machine.
int stateOffEntered_ {0};

class VariantFsmTest : public variantFsm::Fsm<VariantFsmTest, Event, State>,
                       public ::testing::Test
{
protected:
    int stateOffExited_ {0};
    int stateOnEntered_ {0};
    int stateOnExited_ {0};
    int transtionOffToOnTaken_ {0};
    int transtionOnToOnTaken_ {0};

public:
    void entry (Off);
    State transition (Off &&, SwitchOff);
    State transition (Off &&, SwitchOn);
    void exit (TagType<Off>);

    void entry (On);
    State transition (On &&, SwitchOn);
    State transition (On &&, SwitchOff);
    void exit (TagType<On>);

    // Define the default transition for transitions not explicitly
	// defined above. 
	template <typename S, typename E>
    State transition(S&& state, E)
    {
		return std::move (state);
    }

    // As we make use of exit/entry functions, we must handle all cases.
    // The default entry/exit action is an immediate return.
    template <typename S> void exit(variantFsm::TagType<S>) {}
    template <typename S> void entry(S&) {}
};

void VariantFsmTest::entry (Off)
{
    ++stateOffEntered_;
}
 
// In the state Off SwitchOff events are ignored and exit and entry
// actions are not performed.
State VariantFsmTest::transition (Off &&, SwitchOff)
{
    return std::monostate {};
}

State VariantFsmTest::transition (Off &&, SwitchOn)
{
    ++transtionOffToOnTaken_;
    return On {};
}

void VariantFsmTest::exit (TagType<Off>)
{
    ++stateOffExited_;
}

void VariantFsmTest::entry (On)
{
    ++stateOnEntered_;
}


// In the state On SwitchOn events cause an external transition to
// the same state and as it is an external transition entry and exit 
// actions of On are executed.
State VariantFsmTest::transition (On &&, SwitchOn)
{
    ++transtionOnToOnTaken_;
    return On {};
}

State VariantFsmTest::transition (On &&, SwitchOff)
{
    return Off {};
}

void VariantFsmTest::exit (TagType<On>)
{
    ++stateOnExited_;
}


// Verify that on creation of the state machine the entry action of the
// initial state has been executed once.
TEST_F (VariantFsmTest, initialStateEntryActionExecuted)
{
    EXPECT_TRUE (stateOffEntered_ == 1);
    EXPECT_TRUE (stateOffExited_ == 0);

    // Reset stateOffEntered for a next test
    stateOffEntered_ = 0;
}

// Verify that SwitchOff events in the state Off are ignored, i.e.
// the state remains Off and entry and exit actions are not executed.
TEST_F (VariantFsmTest, noEntryExitActionsOnInternalTransition)
{
    EXPECT_TRUE (stateOffEntered_ == 1);
    EXPECT_TRUE (stateOffExited_ == 0);
    dispatch (SwitchOff {});
    EXPECT_TRUE (stateOffEntered_ == 1);
    EXPECT_TRUE (stateOffExited_ == 0);
}

// Verify that on a SwitchOn event in the state Off the following
// actions have been performed:
// - The exit action of Off has been executed,
// - The entry action of On has been executed.
TEST_F (VariantFsmTest, transitionTaken)
{
    EXPECT_TRUE (stateOffExited_ == 0);
    EXPECT_TRUE (stateOnEntered_ == 0);
    EXPECT_TRUE (stateOnExited_ == 0);
    EXPECT_TRUE (transtionOffToOnTaken_ == 0);
    dispatch (SwitchOn {});
    EXPECT_TRUE (transtionOffToOnTaken_ == 1);
    EXPECT_TRUE (stateOffExited_ == 1);
    EXPECT_TRUE (stateOnEntered_ == 1);
    EXPECT_TRUE (stateOnExited_ == 0);
}

TEST_F (VariantFsmTest, entryExitActionsOnExternalTransition)
{
    // Transition to On
    dispatch (SwitchOn {});
    EXPECT_TRUE (transtionOffToOnTaken_ == 1);
    EXPECT_TRUE (stateOffExited_ == 1);
    EXPECT_TRUE (stateOnEntered_ == 1);
    EXPECT_TRUE (stateOnExited_ == 0);

    // An external transition to the same state causes execution of
    // entry and exit actions.
    dispatch (SwitchOn {});
    EXPECT_TRUE (transtionOnToOnTaken_ == 1);
    EXPECT_TRUE (stateOnEntered_ == 2);
    EXPECT_TRUE (stateOnExited_ == 1);
}