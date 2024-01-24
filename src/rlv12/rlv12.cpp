#include "rlv12.h"
#include "types.h"

#include <thread>
#include <chrono>

using std::mutex;
using std::lock_guard;
using std::bind;
using std::placeholders::_1;

// RLV12 constructor
// Set name and default base address and vector. Set the controller
// ready to accept a command.
// The base class protected data members cannot be accessed in an
// initializer list
// ToDo: Remove default RLV12 constructor
RLV12::RLV12 (Qbus *bus)
    :
    PDP11Peripheral (bus),
    csr_ {0},
    bar_ {0},
    dar_ {0},
    bae_ {0},
    dataBuffer_ {nullptr},
    rlType_ {RLConfig::RLType::RLV12},
    _22bit_ {false},
    wordCounter_ {0},
    fifoIndex_ {0}
{
    name_ = "RL";
    baseAddress_ = RLV_BaseAddress;
    vector_ = RLV_Vector;

    // Allocate the transfer buffer, initializing to zero
    dataBuffer_ = new (std::nothrow) u16[maxTransfer]();

    if (dataBuffer_ == nullptr)
        throw ("Allocating memory for transfer buffer failed");

    bus_->BINIT().subscribe (bind (&RLV12::BINITReceiver, this, _1));

    // Start the command processor
    cmdProcessor_ = std::make_unique<CmdProcessor> (this);
}

RLV12::RLV12 (Qbus *bus, shared_ptr<RLConfig> rlConfig)
    :
    PDP11Peripheral (bus),
    csr_ {0},
    bar_ {0},
    dar_ {0},
    bae_ {0},
    dataBuffer_ {nullptr},
    rlType_ {rlConfig->rlType},
    _22bit_ {rlConfig->_22bit},
    wordCounter_ {0},
    fifoIndex_ {0}
{
    name_ = "RL";
    baseAddress_ = (rlConfig->address > 0) ? 
        rlConfig->address : RLV_BaseAddress;
    vector_ = (rlConfig->vector > 0) ? rlConfig->vector : RLV_Vector;

    // Allocate the transfer buffer, initializing to zero
    dataBuffer_ = new (std::nothrow) u16[maxTransfer]();

    if (dataBuffer_ == nullptr)
        throw ("Allocating memory for transfer buffer failed");

    // Attach files to the RL units
	for (size_t unitNumber = 0; 
		unitNumber < rlConfig->numUnits; ++unitNumber)
	{
		shared_ptr<RLUnitConfig> rlUnitConfig = 
            static_pointer_cast<RLUnitConfig> (rlConfig->rlUnitConfig[unitNumber]);

        if (rlUnitConfig == nullptr)
            continue;

		if (unit(unitNumber)->configure (rlUnitConfig) != StatusCode::OK)
			throw "Error attaching " + rlUnitConfig->fileName;
	}

    bus_->BINIT().subscribe (bind (&RLV12::BINITReceiver, this, _1));

    // Start the command processor
    cmdProcessor_ = std::make_unique<CmdProcessor> (this);
}

// Destructor to deallocate transfer buffer
// ToDo: Delete copy constructor and copy assignment operator
RLV12::~RLV12 ()
{
    // Explicitly signal the command processor to stop. This is necessary
    // to prevent Linux pthread scheduling to block on the cmdProcessor
    // thread.
    cmdProcessor_->finish ();

    if (dataBuffer_ != nullptr)
        delete [] dataBuffer_;
}

// On assertion of the BINIT signal initialize the device.
void RLV12::BINITReceiver (bool signalValue)
{
    if (signalValue)
    {
        // Guard against controller register access from writeWord()
        lock_guard<mutex> lock {controllerMutex_};

        reset ();
    }
}

//
// RLV12 device reset
//
// The proper bits of the CSR have to be reset. Those bits are bits 1-6,
// 8-13 and 15. Bit 15 has to be cleared only if bit 14 (Drive Error) is not
// set. Bit 0 (Drive Ready) is a don't care. Source: ZRLGE0 test 14.
// 
// Note that the RL11 does NOT recalibrate its drives on RESET.
// Source: simh comment.
void RLV12::reset ()
{
    constexpr u16 properBits = 037576;

    // Reset CRS
    csr_ &= ~properBits;
    if (!(csr_ & CSR_DriveError))
        csr_ &= ~CSR_CompositeError;

    bar_ = 0;
    dar_ = 0;
    bae_ = 0;
    
    // ToDo: Clear interrupt request
    // CLR_INT(RL);

    // for (RL01_2 unit : units_)
    // {
        // ToDo: Cancel outstanding timer for this unit
        // sim_cancel(uptr);

        // ToDo: Clear unit error condition
        // N.B. STAT == u4
        // uptr->STAT &= ~RLDS_ERR;
    // }
}

// An RLV11 controller or an RLV12 with the 22-bit option disabled has no
// Bus Address Extension register. This function will return true for
// an access to that register; following reads and writes to that register
// however will return a non-existing memory error.
bool RLV12::responsible (BusAddress busAddress)
{
    if (!busAddress.isInIOpage ())
        return false;

    return busAddress.registerAddress () >= baseAddress_ &&
            busAddress.registerAddress () <= (baseAddress_ + BAE) ? 
        true : false;
}

// Update the BA register and the CSR BA16 and BA17 bits from the given
// memory address. In case of an RLV12 (with enabled 22-bit addressing)
// also update the BAE register. For systems without a BAE keep the contents
// of that register zero, so it can be used to calculate a memory address
// for 16-, 18- and 22-bit systems.
void RLV12::memAddrToRegs (u32 memoryAddress)
{
    // Load the lower 16 bits in the BAR, the upper two bits in the RLCS
    // and the upper 6 bits in the BAE
    u16 upper6Bits = memoryAddress >> 16;
    bar_ = memoryAddress & BAR_Mask;
    // rlcs = (rlcs & ~RLCS_MEX) | ((upper6Bits & RLCS_M_MEX) << RLCS_V_MEX);
    csr_ = (csr_ & ~CSR_AddressExtension) | 
        ((upper6Bits & CSR_AddressExtMask) << CSR_AddressExtPosition);

    if (rlType_ == RLConfig::RLType::RLV12 && _22bit_)
        bae_ = upper6Bits & BAE_Mask; 
}

// Get a 16-, 18- or 22-bit address from the BA, CSR and (in case of 22-bit
// systems) BAE registers. For 16- and 18-bit systems the BA16 and BA17 bits
// are used, for 22-bit systems the BAE register is used.
u32 RLV12::memAddrFromRegs ()
{
    if (!(rlType_ == RLConfig::RLType::RLV12 && _22bit_))
        return (getBA16BA17 (csr_) << 16) | bar_;
    else
        return (bae_ << 16) | bar_;
}

// Update the BAE register bits 00 and 01 from the contents of the CSR BA16
// and BA17 bits
void RLV12::updateBAE ()
{
    if (rlType_ == RLConfig::RLType::RLV12 && _22bit_)
        bae_ = (bae_ & ~CSR_AddressExtMask) | 
            ((csr_ >> CSR_AddressExtPosition) & CSR_AddressExtMask);
}

// Get the BA16 and BA17 bits from the given csr_
constexpr u16 RLV12::getBA16BA17 (u16 csr_)
{
    return (csr_ >> CSR_AddressExtPosition) & CSR_AddressExtMask;
}