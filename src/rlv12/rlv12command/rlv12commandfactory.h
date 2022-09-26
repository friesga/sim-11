#ifndef _RLV12COMMANDFACTORY_H_
#define _RLV12COMMANDFACTORY_H

#include "rlv12/rlv12.h"
#include "rlv12command.h"
#include "rlv12getstatuscmd.h"
#include "rlv12maintenancecmd.h"
#include "rlv12readcmd.h"
#include "rlv12readheadercmd.h"
#include "rlv12seekcmd.h"
#include "rlv12writecmd.h"
#include "rlv12writecheckcmd.h"
#include "rlv12readnoheadercmd.h"

#include <memory>

// A template class for the creation of RLV12Command objects
template <typename T>
class CommandFactory
{
public:
    static std::unique_ptr<T> create (int32_t currentDiskAddress, 
        int32_t newDiskAddress, int32_t memoryAddress, int32_t wordCount);
};

// Default creation of RLV12Command objects
// 
// Applicable to:
// - Write Check (Function Code 1),
// - Write Data (Function Code 5),
// - Read Data (Function Code 6)
//
template <typename T>
std::unique_ptr<T> CommandFactory<T>::create 
    (int32_t currentDiskAddress, int32_t newDiskAddress, 
     int32_t memoryAddress, int32_t wordCount)
{
    if (getCylinder (currentDiskAddress) != getCylinder (newDiskAddress) ||
        getSector (currentDiskAddress) >= RL_NUMSC)
        // Bad cylinder or sector
        return {};

    return std::make_unique<T> (newDiskAddress,
        memoryAddress, wordCount);
}

// Create an object of type RLV12ReadNoHeaderCmd
template <>
std::unique_ptr<RLV12ReadNoHeaderCmd> 
    CommandFactory<RLV12ReadNoHeaderCmd>::create 
        (int32_t currentDiskAddress, int32_t newDiskAddress,
         int32_t memoryAddress, int32_t wordCount)
{
    if (getSector (currentDiskAddress) >= RL_NUMSC)
        // Bad sector
        return {};

    return std::make_unique<RLV12ReadNoHeaderCmd> (currentDiskAddress,
        memoryAddress, wordCount);
}

// Create an object of type RLV12SeekCmd
template <>
std::unique_ptr<RLV12SeekCmd> 
    CommandFactory<RLV12SeekCmd>::create 
        (int32_t currentDiskAddress, int32_t newDiskAddress,
         int32_t memoryAddress, int32_t wordCount)
{
    return std::make_unique<RLV12SeekCmd> (currentDiskAddress,
        memoryAddress, wordCount);
}

// Create an object of type RLV12ReadHeaderCmd
template <>
std::unique_ptr<RLV12ReadHeaderCmd> 
    CommandFactory<RLV12ReadHeaderCmd>::create 
        (int32_t currentDiskAddress, int32_t newDiskAddress,
         int32_t memoryAddress, int32_t wordCount)
{
    return std::make_unique<RLV12ReadHeaderCmd> (currentDiskAddress,
        memoryAddress, wordCount);
}

// Create an object of type RLV12MaintenanceCmd
template <>
std::unique_ptr<RLV12MaintenanceCmd> 
    CommandFactory<RLV12MaintenanceCmd>::create 
        (int32_t currentDiskAddress, int32_t newDiskAddress,
         int32_t memoryAddress, int32_t wordCount)
{
    return std::make_unique<RLV12MaintenanceCmd> (currentDiskAddress,
        memoryAddress, wordCount);
}

#endif // !_RLV12COMMANDFACTORY_H_
