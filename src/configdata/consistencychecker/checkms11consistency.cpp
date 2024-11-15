#include "consistencychecker.h"

#include <stdexcept>

using std::out_of_range;
using std::invalid_argument;

// This function checks the consistency of the MSV11 and MS11-P memory
// modules. The function has to be instantiated with the configuration
// class, i.e. MSV11Config or MS11PConfig and the capacity of the module
// in kilobytes.
//
template<typename TConfig, size_t capacity>
void ConsistencyChecker::checkMS11Consistency ()
{
    // Check the maximum number of MSV11-D cards in the system
    size_t const maximumNrOfMS11cards {4};

    vector<shared_ptr<TConfig>> ms11Cards {};

    for (auto device: systemConfig_)
        if (holds_alternative<shared_ptr<TConfig>> (device))
            ms11Cards.push_back (get<shared_ptr<TConfig>> (device));

    // Verify it contains no more than the maximum number of devices
    if (ms11Cards.size () > maximumNrOfMS11cards)
        throw std::out_of_range ("Maximum number of MSV11/MS11-P cards (4) exceeded");

    // Check the adress ranges of the memories don't overlap
    for (auto ms11Card1 : ms11Cards)
    {
        // Check the address range of this card with the other cards
        for (auto ms11Card2 : ms11Cards)
        {
            if (ms11Card2 == ms11Card1)
                continue;

            if (conflictsWith (static_pointer_cast<TConfig> (ms11Card1), 
                static_pointer_cast<TConfig> (ms11Card2), capacity))
                    throw invalid_argument {"MSV11/MS11-P starting address conflict"};
        }
    }
}

template<typename TConfig>
bool ConsistencyChecker::conflictsWith (shared_ptr<TConfig> ms11Card1, 
    shared_ptr<TConfig> ms11Card2, size_t capacity)
{
    return isWithin (ms11Card1->startingAddress, ms11Card2->startingAddress,
        ms11Card2->startingAddress + capacity - 1);
}

bool ConsistencyChecker::isWithin (u32 address, u32 begin, u32 end)
{
    return address >= begin && address <= end;
}

// As the template function checkMS11Consistency() is defined outside of 
// the header file, all template instantiations have to be defined explcitly.
template void ConsistencyChecker::checkMS11Consistency<MSV11Config, 64 * 1024> ();
template void ConsistencyChecker::checkMS11Consistency<MS11PConfig, 1024 * 1024> ();