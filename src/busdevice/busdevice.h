#ifndef _QBUSMODULE_H_
#define _QBUSMODULE_H_

#include "types.h"
#include "statuscodes.h"

#include <string>
#include <memory>

/* Generic Device CSR bits */

#define CSR_V_GO        0                               // Go 
#define CSR_V_IE        6                               // Interrupt Enable
#define CSR_V_DONE      7                               // Done 
#define CSR_V_BUSY      11                              // Busy
#define CSR_V_ERR       15                              // Error
#define CSR_GO          (1u << CSR_V_GO)
#define CSR_IE          (1u << CSR_V_IE)
#define CSR_DONE        (1u << CSR_V_DONE)
#define CSR_BUSY        (1u << CSR_V_BUSY)
#define CSR_ERR         (1u << CSR_V_ERR)

class QBUS;

// Define the functions every QBUS module should provide.
// 
// The class integrates members of the simh-v312-2 Device struct.
class BusDevice
{
protected:
    std::string name_;          // Device name
    u32 baseAddress_;           // Device registers base address
    u32 vector_;                // Device vector

public:
	QBUS*	bus;
	virtual StatusCode read (u16 addr, u16 *destination) = 0;
	virtual StatusCode writeWord (u16 addr, u16 value) = 0;
	virtual StatusCode writeByte (u16 addr, u8 value);
	// virtual void service (Unit &unit) = 0;
	virtual u8 responsible (u16 addr) = 0;
	virtual void reset () = 0;

	std::string name() { return name_; }
};

#endif // _QBUSMODULE_H_