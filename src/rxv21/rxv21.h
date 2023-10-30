#ifndef _RXV21_H_
#define _RXV21_H_

#include "qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "conddata/conddata.h"
#include "configdata/rxv21config/rxv21config.h"
#include "variantfsm/fsm.h"
#include "trace/trace.h"

namespace rxv21
{

	/* RX2CS bits */
	#define	RX_GO				_BV(0)
	#define	RX_FUNCTION_MASK	(_BV(1) | _BV(2) | _BV(3))
	#define	RX_UNIT_SEL			_BV(4)
	#define	RX_DONE				_BV(5)
	#define	RX_INTR_ENB			_BV(6)
	#define	RX_TR				_BV(7)
	#define	RX_DEN				_BV(8)
	#define	RX_RX02				_BV(11)
	#define	RX_EXT_ADDR_MASK	(_BV(12) | _BV(13))
	#define	RX_INIT				_BV(14)
	#define	RX_ERROR			_BV(15)

	// RX2CS Read Mask bits
	#define	RX_RMASK		(RX_UNIT_SEL | RX_DONE | RX_INTR_ENB \
			| RX_TR | RX_DEN | RX_RX02 | RX_ERROR)

	// RX2CS Write Mask bits
	#define	RX_WMASK		(RX_GO | RX_FUNCTION_MASK | RX_UNIT_SEL \
			| RX_INTR_ENB | RX_DEN | RX_EXT_ADDR_MASK | RX_INIT)

	/* RX2CS function codes */
	#define	RX_FILL_BUFFER			(00 << 1)
	#define	RX_EMPTY_BUFFER			(01 << 1)
	#define	RX_WRITE_SECTOR			(02 << 1)
	#define	RX_READ_SECTOR			(03 << 1)
	#define	RX_SET_MEDIA_DENSITY	(04 << 1)
	#define	RX_READ_STATUS			(05 << 1)
	#define	RX_WRITE_DELETED_DATA	(06 << 1)
	#define	RX_READ_ERROR_CODE		(07 << 1)

	/* RX2ES bits */
	#define	RX2ES_CRC			_BV(0)
	#define	RX2ES_ID			_BV(2)
	#define	RX2ES_RX_AC_LO		_BV(3)
	#define	RX2ES_DEN_ERR		_BV(4)
	#define	RX2ES_DRV_DEN		_BV(5)
	#define	RX2ES_DD			_BV(6)
	#define	RX2ES_DRV_RDY		_BV(7)
	#define	RX2ES_UNIT_SEL		_BV(8)
	#define	RX2ES_WC_OVFL		_BV(10)
	#define	RX2ES_NXM			_BV(11)

	#define	RX2ES_DEFAULT		(RX2ES_ID | RX2ES_DRV_DEN | RX2ES_DRV_RDY)
	#define	RX2ES_ERRORS		(RX2ES_CRC | RX2ES_RX_AC_LO | RX2ES_DEN_ERR \
			| RX2ES_WC_OVFL | RX2ES_NXM)

	// Define RXV21 events
	// ToDo: RXV21 events shouldn't be in the global scope
	struct rxv21Init {};
	struct rxv21Go {};
	struct rxv21Rx2dbFilled {};

	// Define RXV21 states
	struct rxv21Idle {};
	struct rxv21FillBufferRx2wc {};
	struct rxv21FillBufferRx2ba {};

	struct rxv21EmptyBufferRx2wc {};
	struct rxv21EmptyBufferRx2ba {};

	struct rxv21WriteSectorRx2sa {};
	struct rxv21WriteSectorRx2ta {};

	struct rxv21ReadSectorRx2sa {};
	struct rxv21ReadSectorRx2ta {};

	struct rxv21ReadErrorCodeRx2ba {};


	// Define events and states to use. The first state member of the variant
	// is the initial state.
	using Event = std::variant<rxv21Init, rxv21Go, rxv21Rx2dbFilled>;
	using State = std::variant<rxv21Idle, 
		rxv21FillBufferRx2wc,  rxv21FillBufferRx2ba, 
		rxv21EmptyBufferRx2wc, rxv21EmptyBufferRx2ba,
		rxv21WriteSectorRx2sa, rxv21WriteSectorRx2ta,
		rxv21ReadSectorRx2sa,  rxv21ReadSectorRx2ta,
		rxv21ReadErrorCodeRx2ba>;

	class RXV21 : public PDP11Peripheral, public variantFsm::Fsm<RXV21, Event, State>
	{
		u16	base;
		u16	vector;

		// All software control of the RX211/RXV21 is performed by means
		// of two device registers: the command and status register (RX2CS)
		// and a multipurpose data buffer register (RX2DB) which have been
		// assigned bus addresses and can be read or loaded.
		u16	rx2cs;	// Control and status register
		u16	rx2db;	// Data Buffer register

		// Contents of the RXV21's "virtual" registers.
		// Note that the term "register" is a misnomer, the contents of
		// these "registers" are read and written via the Data Buffer (RX2DB)
		// register.
		u16	rx2ta;	// RX Track Address
		u16	rx2sa;	// RX Sector Address
		u16	rx2wc;	// RX Word Count Register
		u16	rx2ba;	// RX Bus Address Register
		u16	rx2es;	// RX Error and Status

		u16	error;

		u16	buffer[128];
		u8* data;

		void clearErrors ();
		void done ();
		void fillBuffer ();
		void emptyBuffer ();
		void writeSector ();
		void readSector ();
		void readStatus ();
		void readErrorCode ();

	public:
		RXV21 (Qbus *bus);
		RXV21 (Qbus* bus, shared_ptr<RXV21Config> rxConfig);
		~RXV21 ();
		StatusCode read (u16 address, u16 *destAddress) override;
		StatusCode writeWord (u16 address, u16 value) override;
		bool responsible (u16 address) override;
		void reset ();

		// Declare the signal receivers
		void BINITReceiver (bool signalValue);

		// Definition of state transitions
		State transition (rxv21Idle &&, rxv21Go);

		void entry (rxv21FillBufferRx2wc);
		State transition (rxv21FillBufferRx2wc &&, rxv21Rx2dbFilled);
		State transition (rxv21FillBufferRx2ba &&, rxv21Rx2dbFilled);

		void entry (rxv21EmptyBufferRx2wc);
		State transition (rxv21EmptyBufferRx2wc &&, rxv21Rx2dbFilled);
		State transition (rxv21EmptyBufferRx2ba &&, rxv21Rx2dbFilled);

		void entry (rxv21WriteSectorRx2sa);
		State transition (rxv21WriteSectorRx2sa &&, rxv21Rx2dbFilled);
		State transition (rxv21WriteSectorRx2ta &&, rxv21Rx2dbFilled);

		void entry (rxv21ReadSectorRx2sa);
		State transition (rxv21ReadSectorRx2sa &&, rxv21Rx2dbFilled);
		State transition (rxv21ReadSectorRx2ta &&, rxv21Rx2dbFilled);

		void entry (rxv21ReadErrorCodeRx2ba);
		State transition (rxv21ReadErrorCodeRx2ba &&, rxv21Rx2dbFilled);

		// Reaction to an rxv21Init is equal in all states
		template <typename S>
		State transition(S&& state, rxv21Init)
		{
			reset ();
			return rxv21Idle {};
		}

		// Define the default transition for transitions not explicitly
		// defined above
		template <typename S, typename E>
		State transition(S&& state, E)
		{
			return std::move (state);
		}

		// As we make use of exit/entry functions, we must handle all cases
		template <typename S> void exit(variantFsm::TagType<S>) {}
		template <typename S> void entry(S&) {}
	};
}

#endif // _RXV21_H_