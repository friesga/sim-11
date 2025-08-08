#include "rk05.h"

#include <functional>

using std::bind;

using namespace RKTypes;

// ToDo: Uniform parameters with writeDataToSector()
RKTypes::CommandCompletion RK05::write (DiskAddress diskAddress,
    u16 wordCount, u16* data)
{
    size_t wordsWritten = writeDataToDrive (diskAddress, data,
        wordCount);
    return RKTypes::CommandCompletion {StatusCode::Success,
        wordsWritten, wordsWritten / rk05Geometry_.wordsPerSector ()};
}

size_t RK05::writeDataToDrive (DiskAddress diskAddress, u16* buffer, u32 numWords)
{
    wtIndicator_->show (Indicator::State::On);
    size_t wordsWritten = diskDrive_.writeDataToSector (diskAddress, buffer,
        numWords);
    wtIndicator_->show (Indicator::State::Off);
    return wordsWritten;
}

RKTypes::CommandCompletion RK05::read (DiskAddress diskAddress,
    u16 wordCount, u16* data)
{
    size_t wordsRead = readDataFromDrive (diskAddress, data,
        wordCount);
    return RKTypes::CommandCompletion {StatusCode::Success, wordsRead,
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
RKTypes::CommandCompletion RK05::readHeader (DiskAddress diskAddress,
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
    return RKTypes::CommandCompletion {StatusCode::Success, wordsRead,
        wordsRead};
}

u16 RK05::cylinderFromLBN (u32 lbn)
{
    return lbn / (rk05Geometry_.sectorsPerSurface () *
        rk05Geometry_.numberOfHeads ());
}
