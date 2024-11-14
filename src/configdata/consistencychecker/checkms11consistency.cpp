#include "consistencychecker.h"

#include <stdexcept>

using std::out_of_range;
using std::invalid_argument;

// This function checks the consistency of the MSV11 and MS11-P memory
// modules. The function has to be instantiated with the configuration
// class, i.e. MSV11Config or MS11PConfig.
//
template<typename TConfig>
void ConsistencyChecker::checkMS11Consistency ()
{
    // Check the maximum number of MSV11-D cards in the system
    size_t const maximumNrOfMSV11cards {4};

    vector<shared_ptr<TConfig>> msv11Cards {};

    for (auto device: systemConfig_)
        if (holds_alternative<shared_ptr<TConfig>> (device))
            msv11Cards.push_back (get<shared_ptr<TConfig>> (device));

    // Verify it contains no more than the maximum number of devices
    if (msv11Cards.size () > maximumNrOfMSV11cards)
        throw std::out_of_range ("Maximum number of MSV11/MS11-P cards (4) exceeded");

    // Check the adress ranges of the memories don't overlap
    for (auto msv11Card1 : msv11Cards)
    {
        // Check the address range of this card with the other cards
        for (auto msv11Card2 : msv11Cards)
        {
            if (msv11Card2 == msv11Card1)
                continue;

            if (conflictsWith (static_pointer_cast<TConfig> (msv11Card1), 
                static_pointer_cast<TConfig> (msv11Card2)))
                    throw invalid_argument {"MSV11/MS11-P starting address conflict"};
        }
    }
}

template<typename TConfig>
bool ConsistencyChecker::conflictsWith (shared_ptr<TConfig> msv11Card1, 
    shared_ptr<TConfig> msv11Card2)
{
    u32 const msv11Capacity {64 * 1024};
    return isWithin (msv11Card1->startingAddress, msv11Card2->startingAddress,
        msv11Card2->startingAddress + msv11Capacity - 1);
}

bool ConsistencyChecker::isWithin (u32 address, u32 begin, u32 end)
{
    return address >= begin && address <= end;
}

// As the template function checkMS11Consistency() is defined outside of 
// the header file, all template instantiations have to be defined explcitly.
template void ConsistencyChecker::checkMS11Consistency<MSV11Config> ();