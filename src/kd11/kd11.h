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
using std::mutex;
using std::unique_lock;
using std::defer_lock;
using std::monostate;

namespace kd11_f
{
	// Definition of the events to be processed by the KD11
	struct PowerOk {};
	struct Halt {};
	struct Start {};
	struct Reset {};
	struct PowerDown {};

	using Event = std::variant<PowerOk,
		Halt,
		Start,
		Reset,
		PowerDown>;

	// Definition of the states
	struct PowerOff {};
	struct Running {};
	struct Halted {};

	using State = std::variant<PowerOff,
		Running,
		Halted,
		monostate>;

	// The class KD11 is composed of the KD11 CPU and the KD11 ODT.
	class KD11 : public BusDevice, public variantFsm::Fsm<KD11, Event, State>
	{
	public:
		KD11 (Qbus *bus);
		KD11 (Qbus *bus, shared_ptr<KD11Config> kd11Config);
		void step ();
		void run ();
	
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
		void BDCOKReceiver (bool signalValue);

		State powerUpRoutine ();

		// Definition of the KD11 state machine
		State transition (PowerOff&&, PowerOk);			// -> Halted/Running
		void entry (Running);
		State transition (Running&&, Reset);			// -> Running
		State transition (Running&&, Halt);				// -> Halted
		void entry (Halted);
		State transition (Halted&&, Start);				// -> Running
		State transition (Halted&&, Reset);				// -> Running

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

		// Definition of a queue for the processing of bus signals
		ThreadSafeQueue<Event> signalQueue_;

		// Safe guard against simultaneous CPU access
		// ToDo: To be deleted!
		mutex cpuMutex_;

		InterruptRequest const powerFail {RequestType::Trap, TrapPriority::PowerFail, 0, 024};

		void subscribeToSignals ();
		void waitForBDCOK ();
		void runODT ();
		bool signalAvailable ();
		template <typename T> bool signalIsOfType ();
	};
}
#endif // !_KD11_H_
