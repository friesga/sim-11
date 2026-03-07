#ifndef _PSWBUSDEVICE_H_
#define _PSWBUSDEVICE_H_

#include "abstractbusdevice/abstractbusdevice.h"

// Some processors allow access to the PSW via address 0177776. This
// is implemented via a bus access. Micronote 70 states: "When the PSW is
// explicitly addressed (MOV #340, @#177776), the transfer will appear on the
// bus in a similar fashion to the MMU registers except that there will be no
// reply. Likewise, there can be no connnunication from a bus master to the
// PSW", so this implementation is not completely accurate.
//
// Table 8-1 (Processor Status Word Protection) in EK-KDF11-UG-PR2 states
// that on Explicit PS Access the T-bit is unchanged. Therefore the function
// setPSW() is be used to set the PSW to the new value to prevent the 
// T-bit from being set or cleared.
//
template <typename TCALLER>
class PSWBusDevice : public AbstractBusDevice
{
public:
	// Functions required by the BusDevice interface and not implemented by
	// AbstractBusDevice.
	CondData<u16> read (BusAddress address) override;
	StatusCode writeWord (BusAddress address, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset () override;

private:
	enum { PSWAddress = 0177776 };

    TCALLER& tCaller () noexcept { return static_cast<TCALLER&> (*this); }
    TCALLER const& tCaller () const noexcept { return static_cast<TCALLER const&> (*this); }
};

 template <typename TCALLER>
 CondData<u16> PSWBusDevice<TCALLER>::read (BusAddress busAddress)
 {
     if (busAddress.registerAddress () == PSWAddress)
         return {static_cast<u16> (tCaller ().psw ())};

     return {StatusCode::NonExistingMemory};
 }

 template <typename TCALLER>
 StatusCode PSWBusDevice<TCALLER>::writeWord (BusAddress busAddress, u16 value)
 {
     if (busAddress.registerAddress () == PSWAddress)
     {
         tCaller ().psw ().set (PSW::ProtectionMode::ExplicitAccess, value);
         return StatusCode::Success;
     }

     return StatusCode::NonExistingMemory;
 }

 // Check if the KDF11-A is responsible for the given address. This address can
 // be even or odd. 
 template <typename TCALLER>
 bool PSWBusDevice<TCALLER>::responsible (BusAddress busAddress)
 {
     return ((busAddress.registerAddress () & 0177776) == PSWAddress) ? true : false;
 }

 template <typename TCALLER>
 void PSWBusDevice<TCALLER>::reset ()
 {}

#endif // _PSWBUSDEVICE_H_