#ifndef _INTERRUPTHANDLER_H_
#define _INTERRUPTHANDLER_H_

#include "bus/include/businterrupts.h"
#include "threadsafecontainers/threadsafeprioqueue.h"

#include <functional>

using std::function;

class InterruptHandler
{
public:
	void setInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, unsigned char vector, function<void ()> ack = 0);
	bool containsInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupts ();
	bool intrptReqAvailable ();
	u8 intrptPriority ();
	bool getIntrptReq (InterruptRequest& ir);

private:
	// This queue keeps all interrupt requests, ordered in interrupt priority
	using IntrptReqQueue = ThreadSafePrioQueue<InterruptRequest>;
	IntrptReqQueue intrptReqQueue_;

	void pushInterruptRequest (InterruptRequest interruptReq);
};


#endif // _INTERRUPTHANDLER_H_