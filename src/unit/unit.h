#ifndef _UNIT_H_
#define _UNIT_H_

#include "abstractbusdevice/abstractbusdevice.h"
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
// To avoid dependencies a better place for the WRITE_PROTECT flag would be in
// device specific status flags, but as the unit functions access these flags
// they have to be defined here.
//
enum class Status
{
    WRITE_PROTECT,      // The unit is write protected
    _                   // Required for Bitmask 
};

// Definition of an abstract base class for the units of a device
// ToDo: Rename filePtr to something more meaningful
class Unit
{
public:
    bool isAttached () const;

protected:
    FILE* filePtr_ {nullptr};           // The disk file
    u32 capacity_;                      // Drive capacity in words
    Bitmask<Status> unitStatus_ {};     // Naming discriminate

    // Helper functions for the concrete units
    StatusCode attachUnit (std::string fileName, Bitmask<AttachFlags> flags);
    StatusCode createBadBlockTable (int32_t sectorsPerSurface,
        int32_t physWordsPerSector);


private:
    StatusCode createFile (std::string fileName, Bitmask<AttachFlags> flags);
    StatusCode openReadOnly (std::string fileName);
    StatusCode openReadWrite (std::string fileName);
};

#endif // !_UNIT_H_
