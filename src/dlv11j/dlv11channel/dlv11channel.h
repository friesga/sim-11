#ifndef _DLV11CHANNEL_H_
#define _DLV11CHANNEL_H_

#include "qbus/qbus.h"
#include "statuscodes.h"
#include "configdata/dlv11config/dlv11config.h"
#include "console/console.h"
#include "circular_buffer/circular_buffer.h"
#include "character/character.h"

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
	StatusCode read (BusAddress busAddress, u16 *destAddress);
	StatusCode writeWord (BusAddress busAddress, u16 value);
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

	static const u16 RCSR_READER_ENABLE	 = bitValue (0);
	static const u16 RCSR_RCVR_INT		 = bitValue (6);
	static const u16 RCSR_RCVR_DONE		 = bitValue (7);
	static const u16 RCSR_WR_MASK		 = RCSR_RCVR_INT | RCSR_READER_ENABLE;

	static const u16 RBUF_PARITY_ERROR	 = bitValue (12);
	static const u16 RBUF_FRAMING_ERROR	 = bitValue (13);
	static const u16 RBUF_OVERRUN		 = bitValue (14);
	static const u16 RBUF_ERROR			 = bitValue (15);
	static const u16 RBUF_ERROR_MASK	 = RBUF_OVERRUN | RBUF_FRAMING_ERROR |
										   RBUF_PARITY_ERROR | RBUF_ERROR;
	
	static const u16 XCSR_TRANSMIT_BREAK = bitValue (0);
	static const u16 XCSR_TRANSMIT_INT	 = bitValue (6);
	static const u16 XCSR_TRANSMIT_READY = bitValue (7);
	static const u16 XCSR_WR_MASK		 = XCSR_TRANSMIT_INT | XCSR_TRANSMIT_BREAK;

	u16	rcsr {0};
	u16	rbuf {0};
	u16	xcsr {0};
	u16	xbuf {0};

	u16	baseAddress {0};
	u16	vector {0};

	enum {receiveBufferSize = 1};
	CircularBuffer<u8, receiveBufferSize> receiveBuffer_;

	enum class Function
	{
		Transmit = 0,
		Receive = 1
	};

	u8 interruptPriority (Function function, u16 channelNr);

	Qbus* bus_;
	DLV11Config::Ch3BreakResponse ch3BreakResponse_;
    unsigned char breakKey_;
	u16 channelNr_;
	unique_ptr<Console> console_;
	bool channelRunning_;
	thread transmitterThread_;
	mutex registerAccessMutex_;
	condition_variable transmitter_;
	bool charAvailable_;

	void readChannel ();
	void writeRCSR (u16 value);
	void writeXCSR (u16 value);
	void transmitter ();
	void receive (Character c);
	void processBreak ();
	bool queueCharacter (unsigned char c);
	void receiveDone ();
	void clearReceiveError ();
	void sleepUntil (high_resolution_clock::time_point timePoint);
};

#endif // _DLV11CHANNEL_H_