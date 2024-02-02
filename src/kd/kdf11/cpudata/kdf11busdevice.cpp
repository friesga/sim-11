#include "kdf11cpudata.h"

// Definition of functions required by the BusDevice interface and not
// implemented by AbstractBusDevice.
// 
// The KDF11-A processor allows access to the PSW via address 0177776. This
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
StatusCode KDF11CpuData::read (BusAddress busAddress, u16 *destination)
{
    if (busAddress.registerAddress () == PSWAddress)
    {
        *destination = psw_;
        return StatusCode::OK;
    }

    return StatusCode::NonExistingMemory;
}


StatusCode KDF11CpuData::writeWord (BusAddress busAddress, u16 value)
{
    if (busAddress.registerAddress () == PSWAddress)
    {
        psw_.set (PSW::ProtectionMode::ExplicitAccess, value);
        return StatusCode::OK;
    }

    return StatusCode::NonExistingMemory;
}


// Check if the KDF11-A is responsible for the given address. This address can
// be even or odd. 
bool KDF11CpuData::responsible (BusAddress busAddress)
{
    return ((busAddress.registerAddress () & 0177776) == PSWAddress) ? true : false; 
}


void KDF11CpuData::reset ()
{}