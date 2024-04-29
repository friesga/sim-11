#include "consistencychecker.h"

#include <stdexcept>

using std::out_of_range;
using std::invalid_argument;

void ConsistencyChecker::checkMSV11Consistency ()
{
    // Check the maximum number of MSV11-D cards in the system
    size_t const maximumNrOfMSV11cards {4};

    vector<shared_ptr<MSV11Config>> msv11Cards {};

    for (auto device: systemConfig_)
        if (holds_alternative<shared_ptr<MSV11Config>> (device))
            msv11Cards.push_back (get<shared_ptr<MSV11Config>> (device));

    // Verify it contains no more than the maximum number of devices
    if (msv11Cards.size () > maximumNrOfMSV11cards)
        throw std::out_of_range ("Maximum number of MSV11 cards (4) exceeded"); 

    // Check the adress ranges of the memories don't overlap
    for (auto msv11Card1 : msv11Cards)
    {
        // Check the address range of this card with the other cards
        for (auto msv11Card2 : msv11Cards)
        {
            if (msv11Card2 == msv11Card1)
                continue;

            if (conflictsWith (static_pointer_cast<MSV11Config> (msv11Card1), 
                static_pointer_cast<MSV11Config> (msv11Card2)))
                    throw invalid_argument {"MSV11 starting address conflict"}; 
        }
    }
}

bool ConsistencyChecker::conflictsWith (shared_ptr<MSV11Config> msv11Card1, 
    shared_ptr<MSV11Config> msv11Card2)
{
    u32 const msv11Capacity {64 * 1024};
    return isWithin (msv11Card1->startingAddress, msv11Card2->startingAddress,
        msv11Card2->startingAddress + msv11Capacity - 1);
}

bool ConsistencyChecker::isWithin (u32 address, u32 begin, u32 end)
{
    return address >= begin && address <= end;
}