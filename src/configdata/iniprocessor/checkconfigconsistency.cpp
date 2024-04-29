#include "iniprocessor.h"

#include <vector>
#include <algorithm>
#include <ranges>

using std::vector;
using std::back_inserter;
using std::ranges::view;
using std::ranges::find_if;
using std::copy_if;
using std::invalid_argument;
using std::holds_alternative;
using std::get;

// This function checks the consistency of the configuration across all
// configured devices.
void IniProcessor::checkConfigConsistency ()
{
    checkMSV11Consistency ();
    checkKDF11_BConsistency ();
}

void IniProcessor::checkMSV11Consistency ()
{
    // Check the maximum number of MSV11-D cards in the system
    size_t const maximumNrOfMSV11cards {4};

    vector<shared_ptr<MSV11Config>> msv11Cards {};

    // Copy the MSV11Config's to another vector
    /*
    copy_if (systemConfig.begin (), systemConfig.end (),
        back_inserter (msv11Cards),
        [] (DeviceConfig device) {return holds_alternative<shared_ptr<MSV11Config>> (device);});
    */
    for (auto device: systemConfig)
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

// Check that not both a KDF11-B and a BDV11 are configured. This would lead
// to a system with undefined behaviour.
void IniProcessor::checkKDF11_BConsistency ()
{
    auto isKDF11_B = [] (DeviceConfig device)
        {return holds_alternative<shared_ptr<KDF11_BConfig>> (device);};
    auto isBDV11 = [] (DeviceConfig device)
        {return holds_alternative<shared_ptr<BDV11Config>> (device);};

    if (find_if (systemConfig, isKDF11_B) != systemConfig.end () &&
        find_if (systemConfig, isBDV11)   != systemConfig.end ())
            throw invalid_argument {"Specify either KDF11-B or BDV11"}; 
}

bool IniProcessor::conflictsWith (shared_ptr<MSV11Config> msv11Card1, 
    shared_ptr<MSV11Config> msv11Card2)
{
    u32 const msv11Capacity {64 * 1024};
    return isWithin (msv11Card1->startingAddress, msv11Card2->startingAddress,
        msv11Card2->startingAddress + msv11Capacity - 1);
}

bool IniProcessor::isWithin (u32 address, u32 begin, u32 end)
{
    return address >= begin && address <= end;
}
