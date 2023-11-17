#ifndef _DLV11CHANNEL_H_
#define _DLV11CHANNEL_H_

#include "qbus/qbus.h"
#include "statuscodes.h"
#include "configdata/dlv11config/dlv11config.h"
#include "console/console.h"
#include "circular_buffer/circular_buffer.h"

#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using std::function;
using std::unique_ptr;
using std::shared_ptr;
using std::thread;
using std::mutex;
using std::condition_variable;
using std::chrono::high_resolution_clock;

class DLV11Channel
{
public:
	DLV11Channel (Qbus* bus, u16 channelBaseAddress, u16 vector,
		shared_ptr<DLV11Config> dlv11Config);
	~DLV11Channel ();
	StatusCode read (u16 registerAddress, u16 *destAddress);
	StatusCode writeWord (u16 registerAddress, u16 value);
	void reset ();

private:
	// Define DLV11-J's registers as offsets from the controllers base address
    enum
    { 
        RCSR = 00,	// Receiver control/status register
        RBUF = 02,	// Receiver data buffer register
        XCSR = 04,	// Transmitter control/status register
        XBUF = 06	// Transmitter data buffer register
    };

	u16	rcsr {0};
	u16	rbuf {0};
	u16	xcsr {0};
	u16	xbuf {0};

	u16	baseAddress {0};
	u16	vector {0};

	enum {rBufferSize = 2048};
	CircularBuffer<u8, rBufferSize> rBuffer_;

	Qbus* bus_;
	DLV11Config::Ch3BreakResponse ch3BreakResponse_;
    unsigned char breakKey_;
	u16 channelNr_;
	unique_ptr<Console> console_;
	bool channelRunning_;
	thread transmitterThread_;
	mutex registerAccessMutex_;
	condition_variable dataAvailable_;

	void readChannel ();
	void writeRCSR (u16 value);
	void writeXCSR (u16 value);
	void transmitter ();
	void receive (unsigned char c);
	void processBreak ();
	bool queueCharacter (unsigned char c);
	void receiveDone ();
	void sleepUntil (high_resolution_clock::time_point timePoint);
};

#endif // _DLV11CHANNEL_H_