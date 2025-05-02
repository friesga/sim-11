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

// Definition of an abstract base class for the units of a device
// ToDo: Rename filePtr to something more meaningful
class Unit
{
public:
    bool isAttached () const;
    void setWriteProtected (bool writeProtected);
    bool isWriteProtected () const;

protected:
    FILE* filePtr_ {nullptr};           // The disk file
    

    // Helper functions for the concrete units
    StatusCode attachUnit (std::string fileName, Bitmask<AttachFlags> flags);
    StatusCode createBadBlockTable (int32_t sectorsPerSurface,
        int32_t physWordsPerSector, u32 capacity);


private:
    bool writeProtected_ {false};

    StatusCode createFile (std::string fileName, Bitmask<AttachFlags> flags);
    StatusCode openReadOnly (std::string fileName);
    StatusCode openReadWrite (std::string fileName);
};

#endif // !_UNIT_H_
