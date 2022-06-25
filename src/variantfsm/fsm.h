//! @file Presents an interface to build specialized finite state machines.
//! 
//! This code is based on the article "Sum types and state machines in C++17"
//! by Colin (http://mooooo.ooo/sumtypes-and-statemachines/) and the code at
//! http://mooooo.ooo/sumtypes-and-statemachines/fsm.h. The following changes
//! have been made:
//! - Put code in separate namespace
//! - Replaced include <utility> by include <variant>
//! - Function onEvent() renamed to transition()
//! - Execute entry() for the initial state
//! - Renamed onEntryState() and onExitState() to respectively entry() and
//!   exit(), in accordance with UML terminology
//!
//! The FSM may be composed of some persistent member variables (shared across
//! all states) and contextual state (accessible only to one state).
//!
//! State machines are event-driven; when the FSM receives an event, it will
//! dispatch it to the appropriate handler based on the current state, which
//! will perform some action (optionally modifying some shared state) and
//! return a new state variant.
//!
//! *If* the event causes a change into a new state type, then exit and entrance
//! handlers will be called to optionally perform setup/teardown actions. In
//! this case, the callgraph looks like:
//!
//! ```
//! fsm.dispatch(const Event& myEvent)
//!   -> NewState fsmState = fsm.transition(OldState&& fsmState, const Event& myEvent)
//!   -> fsm.exit<OldState>()
//!   -> fsm.entry(NewState& fsmState)
//! ```
//!
//! A deriving class must implement an `onEvent(StateT&& state, const EventT& event)`
//! for every combination of (StateT, EventT).
//! Implementing `exit(TagType<StateT>)` and `entry(StateT& newState)`
//! is optional, but if implemented the implementation must cover all possible StateT's.
//!
//! Example use:
//! ```
//! /// Number of unique items the vending machine can hold.
//! constexpr int NUM_ITEMS = 40;
//! 
//! /// Data that a caller will supply us when the user presses a button on the vending machine.
//! struct ButtonPressed {
//!     int index;
//! };
//! struct CoinInserted {
//!     /// Value of the coin, in pennies.
//!     int value;
//! };
//! 
//! /// List of errors we might encounter.
//! enum class ErrorType {
//!     OutOfStock,
//!     InsufficientFunds
//! };
//! 
//! // Three different states our vending machine might be in.
//! struct Idle {
//!     /// Amount of cash (in pennies) deposited by the user so far.
//!     int funds;
//! };
//! struct Errored {
//!     ErrorType error;
//! };
//! struct FetchingItem {
//!     int itemRow, itemCol;
//! };
//! 
//! using Event = std::variant<ButtonPressed, CoinInserted>;
//! using State = std::variant<Idle, Errored, FetchingItem>;
//! 
//! struct VendingMachine : public Fsm<VendingMachine, Event, State> {
//!     // Variables that should be shared across all states:
//!     int stock[NUM_ITEMS];
//!     int prices[NUM_ITEMS];
//!     // What to do when in the Idle state and a ButtonPressed event occurs.
//!     State transition(Idle&& state, ButtonPressed event) {
//!         int cost = prices[event.index];
//!         if (stock[event.index] <= 0) {
//!             return Errored{ErrorType::OutOfStock};
//!         } else if (cost > state.funds) {
//!             return Errored{ErrorType::InsufficientFunds};
//!         } else {
//!             giveChange(state.funds - cost);
//!             --stock[event.index];
//!             return FetchingItem{event.index/8, event.index%8};
//!         }
//!     }
//!     // Ignore button presses in all other states.
//!     State transition(FetchingItem&& state, ButtonPressed) {
//!         return std::move(state);
//!     }
//!     State transition(Errored&& state, ButtonPressed) {
//!         return std::move(state);
//!     }
//!     State transition(Idle&& state, CoinInserted event) {
//!         state.funds += event.value;
//!         return std::move(state);
//!     }
//!     // Spit the coins back out in non-idle states.
//!     // Note: we can use a template here to avoid repetitition.
//!     // C++ function overloads work such that events associated with the Idle
//!     // state will be processed by the explicitly defined function above instead
//!     // of this template.
//!     template <typename S> State transition(S&& state, CoinInserted event) {
//!         giveChange(event.value);
//!         return std::move(state);
//!     }
//! 
//!     // Power supply management for whichever circuits fetch the item.
//!     void exit(TagType<FetchingItem>) {
//!         disablePowerSupplies();
//!     }
//!     void entry(FetchingItem&) {
//!         enablePowerSupplies();
//!     }
//! 
//!     // Note that if you make use of the exit/enter handlers, you must explicitly
//!     // handle all cases as we did with the transition handler:
//!     template <typename S> void exit(TagType<S>) {}
//!     template <typename S> void entry(S&) {}
//! private:
//!     void giveChange(int value);
//!     void disablePowerSupplies();
//!     void enablePowerSupplies();
//! };
//! ```

#ifndef _FSM_H_
#define _FSM_H_

#include <variant>

namespace variantFsm
{
    // Struct to remember a value's type.
    template <typename T> struct TagType {};

    // Helper function used by `getVariantTag()`.
    template <typename ...Ts> struct GetVariantTagVisitor
    {
        template <typename T> std::variant<TagType<Ts>...> operator()(const T&)
        {
            return TagType<T>();
        }
    };

    // Turn a variant<T1, T2, ...> into a variant<TagType<T1>, TagType<T2>, ...>.
    // Essentially, this extracts a variant's type information, but ignore any
    // associated data.
    template <typename ...Ts> std::variant<TagType<Ts>...>
    getVariantTag(const std::variant<Ts...>& value)
    {
        return std::move(std::visit(GetVariantTagVisitor<Ts...>(), value));
    }

    // Calls the appropriate `onEvent(State, Event)` function of the derived class.
    template <typename Implementor, typename State> struct EventDispatcher
    {
        EventDispatcher(Implementor& self_) : self(self_) {}
        template <typename ActiveState, typename Event>
        State operator()(ActiveState&& state, const Event& event)
        {
            return std::move (self.transition(std::move(state), event));
        }
        Implementor& self;
    };

    // Calls the appropriate `exit<State>()` function of the derived class.
    template <typename Implementor> struct ExitDispatcher
    {
        ExitDispatcher(Implementor& self_) : self(self_) {}
        template <typename OldState> void operator()(TagType<OldState> tag)
        {
            self.exit(tag);
        }
        Implementor& self;
    };

    // Calls the appropriate `entry(State)` function of the derived class.
    template <typename Implementor> struct EntranceDispatcher
    {
        EntranceDispatcher(Implementor& self_) : self(self_) {}
        template <typename NewState> void operator()(NewState& newState)
        {
            self.entry(newState);
        }
        Implementor& self;
    };

    // Base type for all Finite State Machines.
    template <typename Implementor, typename EventVariant, typename StateVariant>
    struct Fsm
    {
        // public typedefs for any implementors or users of the class.
        using Event = EventVariant;
        using State = StateVariant;

        // Execute the entry exit for the initial state
        Fsm ()
        {
            Implementor* self = static_cast<Implementor*>(this);
            EntranceDispatcher<Implementor> entranceDispatcher(*self);
            std::visit(entranceDispatcher, fsmState);
        }

        // Dispatch an event variant to the appropriate handler, based on the current state.
        void dispatch(const Event& event)
        {
            // Save the type information of the old state, so we can determine if
            // a transition occurs
            auto stateId = fsmState.index();
            Implementor* self = static_cast<Implementor*>(this);
            auto oldStateTag = getVariantTag(fsmState);

            {
                // Dispatch the event and obtain the new state.
                EventDispatcher<Implementor, State> dispatcher(*self);
                this->fsmState = std::move(std::visit(dispatcher, 
                    std::move(fsmState), event));
            }

            if (fsmState.index() != stateId)
            {
                // After a state transition, we need to first call the exit handler,
                // and then the entrance handler of the new state.
                ExitDispatcher<Implementor> exitDispatcher(*self);
                std::visit(exitDispatcher, oldStateTag);

                EntranceDispatcher<Implementor> entranceDispatcher(*self);
                std::visit(entranceDispatcher, fsmState);
            }
        }

    protected:
        // Default handler for when states are exited.
        // This is called after the event is dispatched and before the appropriate
        // `entry()` is called, and only if the `onEvent` function results
        // in a different type than the previous state.
        // At this point, the old state has already been destructed and its data
        // is inaccessible. The TagType serves as a way for the implementor to
        // specialize an otherwise void function.
        template <typename OldState> void exit(TagType<OldState>) {}

        // Default handler for when new states are entered.
        // This is called immediately after `exit()`.
        template <typename NewState> void entry(NewState&) {}

    private:
        State fsmState;

        friend struct ExitDispatcher<Implementor>;
        friend struct EntranceDispatcher<Implementor>;
    };
}
#endif // !_FSM_H_