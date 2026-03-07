#include "rk05.h"

#include <functional>
#include <algorithm>

using std::bind;
using std::min;

using namespace RKTypes;

// Data is written to the disk as complete sectors.
// 
// Short portions (less than 256 data words) of a sector may be read or
// written as long as this short sector is the last sector of the data
// transfer. When a short sector is written, the remainder of the sector is
// automatically written with zeroes. (EK-RK11D-MM-002, p.3-9)
// 
// ToDo: Uniform parameters with writeDataToSector()
//
RKTypes::FunctionResult RK05::write (DiskAddress diskAddress,
    u16 wordCount, u16* data)
{
    size_t wordsWritten = writeDataToDrive (diskAddress, data,
        wordCount);
    return RKTypes::FunctionResult {StatusCode::Success,
        wordsWritten, wordsWritten / rk05Geometry_.wordsPerSector ()};
}

// Write data to the disk as a number of complete sectors. The returned
// number of words written cannot be larger than the original word count.
//
size_t RK05::writeDataToDrive (DiskAddress diskAddress, u16* buffer, u16 numWords)
{
    wtIndicator_->show (Indicator::State::On);

    clearBufferToEndOfSector (buffer, numWords);

    u16 wordsWritten = diskDrive_.writeDataToSector (diskAddress, buffer,
        RKTypes::wordCountForEntireSectors (numWords));

    wtIndicator_->show (Indicator::State::Off);
    return min (wordsWritten, numWords);
}

void RK05::clearBufferToEndOfSector (u16* buffer, u16 numWords)
{
    for (u16 index = numWords;
        index < RKTypes::wordCountForEntireSectors (numWords); ++index)
    {
        buffer[index] = 0;
    }
}

RKTypes::FunctionResult RK05::read (DiskAddress diskAddress,
    u16 wordCount, u16* data)
{
    size_t wordsRead = readDataFromDrive (diskAddress, data,
        wordCount);
    return RKTypes::FunctionResult {StatusCode::Success, wordsRead,
        wordsRead / rk05Geometry_.wordsPerSector ()};
}

size_t RK05::readDataFromDrive (DiskAddress diskAddress, u16* buffer,
    u32 wordCount)
{
    rdIndicator_->show (Indicator::State::On);
    size_t wordsRead = diskDrive_.readDataFromSector (diskAddress, buffer,
        wordCount);
    rdIndicator_->show (Indicator::State::Off);
    return wordsRead;
}

void RK05::seek (u16 cylinderAddress)
{
    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, cylinderAddress),
        [&] {controller_->reportSeekComplete (SeekCompleteReport {driveId_,
            driveError_}); }});

    // The current cylinder address actually should be set only when the
    // seek is completed, but as the seek cannot fail and the new cylinder
    // isn't available in the state machine transition we'll set it now.
    currentCylinderAddress_ = cylinderAddress;
}

// The RKCS FMT bit alters the normal Read operation in that only one word,
// the header word, is transferred to memory per sector. For example,
// a 3-word Read function in Format mode will transfer header words from three
// consecutive sectors to three consecutive memory locations for software checking.
// (EK-RK11D-MM-002, p 3-6).
// 
// The header word contains just the cylinder address in the format of the
// RKDA (i.e. bits 5-12 filled).
// 
// This function reads the header from the given number of sectors, starting
// at the given disk address. The header isn't actually read (it isn't stored
// in the disk file) but is simply extracted from the disk address and stored
// in the correct format.
//
RKTypes::FunctionResult RK05::readHeader (DiskAddress diskAddress,
    u16 wordCount, u16* data)
{
    size_t wordsRead = 0;
    RKDA rkda {};

    for (u32 lbn = rk05Geometry_.LBN (diskAddress); wordsRead < wordCount;
        ++wordsRead, ++lbn)
    {
        rkda.cylinderAddress = cylinderFromLBN (lbn);
        data[wordsRead] = rkda.value;
    }

    // One word per sector is read, so the number of sectors equals the
    // number of words read.
    return RKTypes::FunctionResult {StatusCode::Success, wordsRead,
        wordsRead};
}

u16 RK05::cylinderFromLBN (u32 lbn)
{
    return lbn / (rk05Geometry_.sectorsPerSurface () *
        rk05Geometry_.numberOfHeads ());
}
