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
		u8 functionOrder, u16 vector, function<void ()> requestGrant = 0);
	bool containsInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupts ();
	bool intrptReqAvailable ();
	u8 intrptPriority ();
	bool getIntrptReq (InterruptRequest& ir);

private:
	// The IntrptReqQueue_ keeps track of all interrupt requests, ordered in
	// interrupt priority. The queue needs a multiset as the underlying type
	// as the queue must be able to contain multiple interrupt requests coming
	// from the same device and thus having the same contents.
	using IntrptReqQueue = ThreadSafePrioQueue<InterruptRequest,
		std::multiset<InterruptRequest>>;
	IntrptReqQueue intrptReqQueue_;

	void pushInterruptRequest (InterruptRequest interruptReq);
};


#endif // _INTERRUPTHANDLER_H_