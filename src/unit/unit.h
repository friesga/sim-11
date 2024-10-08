#ifndef _UNIT_H_
#define _UNIT_H_

#include "pdp11peripheral/pdp11peripheral.h"
#include "statuscodes.h"
#include "types.h"
#include "bitmask.h"
#include "attachflags.h"
#include "configdata/deviceconfig/deviceconfig.h"

#include <cstdio>
#include <string>
#include <memory>

using std::shared_ptr;

// Unit status flags. These flags are used in the definition of 
// Bitmask<Status> and provide a compile-time type safety for the use
// of these flags.
// The flags are used for configuration and/or run-time status. This cannot
// be separated easily as some configuration flags (e.g. UNIT_RO) are updated
// run-time to reflect the actual situation.
//
// ToDo: Clean up unused flags
//
enum class Status
{
    UNIT_RO,            /* read only */
    UNIT_ATT,           /* attached */
    _                   /* Required for Bitmask */
};

// Definition of an abstract base class for the units of a device
// ToDo: Rename filePtr to something more meaningful
class Unit
{
    std::string fileName_;      // Name of attached file
    u16 *fileBuffer_;           // Memory pointer for buffered I/O
    size_t hwmark_;             // High water mark
    int32_t position_;          // File position

    StatusCode createFile (std::string fileName, Bitmask<AttachFlags> flags);
    StatusCode openPipe (std::string fileName);
    StatusCode openReadOnly (std::string fileName);
    StatusCode openReadWrite (std::string fileName);

protected:
    PDP11Peripheral *owningDevice_;       // Pointer to the controller
    FILE *filePtr_;                 // The disk file
    u32 capacity_;                  // Drive capacity in words
    u32 flags_;                     // Bit flags
    Bitmask<Status> unitStatus_;    // Naming discriminate

    // Helper functions for the concrete units
    StatusCode attach_unit (std::string fileName, Bitmask<AttachFlags> flags);
    StatusCode createBadBlockTable (int32_t sec, int32_t wds);

public:
    // Constructor
    Unit (PDP11Peripheral *owningDevice);
};

#endif // !_UNIT_H_
