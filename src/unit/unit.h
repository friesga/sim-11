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
    UNIT_ATTABLE,       /* attachable */
    UNIT_RO,            /* read only */
    UNIT_FIX,           /* fixed capacity */
    UNIT_SEQ,           /* sequential */
    UNIT_ATT,           /* attached */
    UNIT_BINK,          /* K = power of 2 */
    UNIT_BUFABLE,       /* bufferable */
    UNIT_MUSTBUF,       /* must buffer */
    UNIT_BUF,           /* buffered */
    UNIT_ROABLE,        /* read only ok */
    UNIT_DISABLE,       /* disable-able */
    UNIT_DIS,           /* disabled */
    UNIT_RAW,           /* raw mode */
    UNIT_TEXT,          /* text mode */
    UNIT_IDLE,          /* idle eligible */

    // Unit dynamic flags (dynflags) (from simh 4.0)
    // These flags are only set dynamically
    UNIT_ATTMULT,       /* allow multiple ATT cmd */
    UNIT_PIPE,          /* file is a pipe */
    UNIT_EXTEND,        /* extended SIMH tape format is enabled */
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
    bool isPipe (std::string fileName);
    StatusCode openReadOnly (std::string fileName);
    StatusCode openReadWrite (std::string fileName);
    void setBuffered ();

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

    // Functions to be implemented by concrete devices
    virtual StatusCode configure (shared_ptr<DeviceConfig> deviceConfig) = 0;
};

#endif // !_UNIT_H_
