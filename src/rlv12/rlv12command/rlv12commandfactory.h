#ifndef _RLV12COMMANDFACTORY_H_
#define _RLV12COMMANDFACTORY_H

#include "rlv12/rlv12.h"
#include "rlv12command.h"
#include "rlv12readcmd.h"
#include "rlv12readheadercmd.h"
#include "rlv12writecmd.h"
#include "rlv12writecheckcmd.h"
#include "rlv12readnoheadercmd.h"

#include <memory>

// A template class for the creation of RLV12Command objects
template <typename T>
class CommandFactory
{
public:
    static std::unique_ptr<T> create (int32_t currentTrackHeadSector, 
        int32_t newTrackHeadSector, int32_t memoryAddress, int32_t wordCount);
};

// Create an object of another type than READNOHEADER
template <typename T>
std::unique_ptr<T> CommandFactory<T>::create 
    (int32_t currentTrackHeadSector, int32_t newTrackHeadSector, 
     int32_t memoryAddress, int32_t wordCount)
{
    if (getCylinder (currentTrackHeadSector) != getCylinder (newTrackHeadSector) ||
        getSector (currentTrackHeadSector) >= RL_NUMSC)
        // Bad cylinder or sector
        return {};

    return std::make_unique<T> (
        getTrack (newTrackHeadSector),
        getSector (newTrackHeadSector),
        memoryAddress, wordCount);
}

// Create an object of type READNOHEADER
template <>
std::unique_ptr<RLV12ReadNoHeaderCmd> 
    CommandFactory<RLV12ReadNoHeaderCmd>::create 
        (int32_t currentTrackHeadSector, int32_t newTrackHeadSector,
         int32_t memoryAddress, int32_t wordCount)
{
    if (getSector (currentTrackHeadSector) >= RL_NUMSC)
        // Bad sector
        return {};

    return std::make_unique<RLV12ReadNoHeaderCmd> (
        getTrack (currentTrackHeadSector),
        getSector (currentTrackHeadSector),
        memoryAddress, wordCount);
}

#endif // !_RLV12COMMANDFACTORY_H_
