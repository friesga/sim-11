#ifndef _KD11_H_
#define _KD11_H_

#include "cpu/kd11cpu.h"
#include "odt/kd11odt.h"
#include "configdata/kd11config/kd11config.h"
#include "variantfsm/fsm.h"
#include "threadsafecontainers/threadsafequeue.h"

using namespace KD11_ODT;

#include <memory>

using std::unique_ptr;
using std::shared_ptr;
using std::unique_lock;
using std::defer_lock;
using std::monostate;

namespace kd11_na
{
	// Definition of the events to be processed by the KD11
	struct BPOK_high {};
	struct Halt {};
	struct Start {};
	struct Reset {};
	struct BPOK_low {};
	struct BDCOK_low {};
	struct Exit {};

	using Event = std::variant<BPOK_high,
		Halt,
		Start,
		Reset,
		BPOK_low,
		BDCOK_low,
		Exit>;

	// Definition of the states
	struct PowerOff {};
	struct Running {};
	struct Halted {};
	struct PowerFail {};
	struct ExitPoint {};

	using State = std::variant<PowerOff,
		Running,
		Halted,
		PowerFail,
		ExitPoint,
		monostate>;

	// The class KD11 is composed of the KD11 CPU and the KD11 ODT.
	class KD11 : public BusDevice, public variantFsm::Fsm<KD11, Event, State>
	{
	public:
		KD11 (Qbus *bus);
		KD11 (Qbus *bus, shared_ptr<KD11Config> kd11Config);
		~KD11 ();
		void start (u16 startAddress);
		void start ();
		
		// Give main() access to the CPU to set PC and runState
		KD11CPU &cpu();

		// The KD11 is a BusDevice without registers so the read and write 
		// register functions are dummies. The reset functie is called on a
		// bus reset and has no function for the KD11 either.
		StatusCode read (u16 addr, u16 *destination) override 
			{ return StatusCode::FunctionNotImplemented; };
		StatusCode writeWord (u16 addr, u16 value) override
			{ return StatusCode::FunctionNotImplemented; };
		StatusCode writeByte (u16 addr, u8 value) override
			{ return StatusCode::FunctionNotImplemented; };
		bool responsible (u16 addr) override { return false; }
		void reset() override {};

		// Declare the signal receivers
		void BHALTReceiver (bool signalValue);
		void BPOKReceiver (bool signalValue);
		void ExitReceiver (bool signalValue);
		void ResetReceiver (bool signalValue);

		// Definition of the KD11 state machine
		State transition (PowerOff&&, BPOK_high);		// -> Halted/Running
		void entry (Running);
		State transition (Running&&, Reset);			// -> Halted/Running
		State transition (Running&&, Halt);				// -> Halted
		State transition (Running&&, BPOK_low);			// -> PowerFail
		void entry (Halted);
		State transition (Halted&&, Start);				// -> Running
		State transition (Halted&&, Reset);				// -> Halted/Running
		State transition (Halted&&, BPOK_low);			// -> PowerOff
		void entry (PowerFail);
		State transition (PowerFail&&, BDCOK_low);		// -> PowerOff
		State transition (PowerFail&&, Halt);			// -> PowerOff

		template <typename S>
        State transition (S&& state, Exit)
        {
			return ExitPoint {};
		}
		void entry (ExitPoint);

		// Define the default transition for transitions not explicitly
        // defined above. The default transition implies the event is ignored.
        template <typename S, typename E>
        State transition (S&& state, E)
        {
			return monostate {};
		}

        // As we make use of exit/entry functions, we must handle all cases.
        // The default entry/exit action is an immediate return.
        template <typename S> void exit (variantFsm::TagType<S>) {}
        template <typename S> void entry (S&) {}

	private:
		KD11CPU cpu_ {bus_};
		unique_ptr<KD11ODT>	odt_ {};
		KD11Config::PowerUpMode powerUpMode_;
		bool kd11Running_;
		u16 startAddress_;

		enum {stdBootAddress = 0173000};

		// The KD11 is started in its own thread
		std::thread kd11Thread_;

		// Definition of a queue for the processing of bus signal events
		ThreadSafeQueue<Event> signalEventQueue_;

		InterruptRequest const powerFail {RequestType::Trap, TrapPriority::PowerFail, 0, 024};

		State powerUpRoutine ();
		void subscribeToSignals ();
		void run ();
		void runODT ();
		bool signalAvailable ();
		template <typename T> bool signalIsOfType ();
	};

	// This function returns true if the first element in the signal queue is
	// a variant holding an Event of the specified type.
	template <typename T>
	bool KD11::signalIsOfType ()
	{
		return holds_alternative<T> (signalEventQueue_.first ());
	}
}

#endif // !_KD11_H_
