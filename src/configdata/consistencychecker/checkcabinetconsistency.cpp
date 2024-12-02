#include "consistencychecker.h"

#include <stdexcept>

using std::out_of_range;
using std::invalid_argument;

// Check the (consistency of) the cabinet specifications for (currently)
// the BA11-N and RL01/02 drives.
// 
// For now the the only supported cabinet one H9642 or equivalent. This
// cabinet has a height of 20 rack units. The top position therefore is
// 19 RU. A BA-11N has a height of 3 RU, an RL01/02 has a height of 6 RU.
//
void ConsistencyChecker::checkCabinetConsistency ()
{ 
    // Definition of the cabinet to keep track of occupied positions in
    // the cabinet. 
    Cabinet h9642Cabinet {"h9642", 20_ru};

    for (auto device : systemConfig_)
    {
        if (holds_alternative<shared_ptr<BA11_NConfig>> (device))
        {
            shared_ptr<BA11_NConfig> ba11nConfig = 
                get<shared_ptr<BA11_NConfig>> (device);

            // The following checks are performed by addUnit too, but
            // executing these checks here too gives the opportunity to
            // generate appropriate error messages.
            if (h9642Cabinet.sectionOutOfRange (ba11nConfig->cabinetPosition,
                        BA11_NConfig::unitHeight) ||
                    ba11nConfig->cabinetPosition->cabinetNr != 0)
                throw out_of_range {"BA11-N cabinet position out of range"};

            if (h9642Cabinet.sectionOccupied (ba11nConfig->cabinetPosition,
                    BA11_NConfig::unitHeight))
                throw invalid_argument {"BA11-N cabinet position already occupied"};

            // The addUnit() shouldn't fail but just to make sure.
            if (!h9642Cabinet.addUnit (ba11nConfig->cabinetPosition,
                    BA11_NConfig::unitHeight))
                throw invalid_argument {"BA11-N couldn't be added to the cabinet"};
        }
        else
        {
            if (holds_alternative<shared_ptr<RLV12Config>> (device))
            {
                shared_ptr<RLV12Config> rlConfig =
                    get<shared_ptr<RLV12Config>> (device);

                // Check the cabinet position for every configured RL01/02
                for (size_t unitNumber = 0;
                    unitNumber < rlConfig->numUnits; ++unitNumber)
                {
                    shared_ptr<RLUnitConfig> rlUnitConfig =
                        rlConfig->rlUnitConfig[unitNumber];

                    if (rlUnitConfig == nullptr)
                        continue;

                    // See comment above
                    if (h9642Cabinet.sectionOutOfRange (rlUnitConfig->cabinetPosition,
                                RLUnitConfig::unitHeight) ||
                            rlUnitConfig->cabinetPosition->cabinetNr != 0)
                        throw out_of_range {"RL01/02 cabinet position out of range"};

                    if (h9642Cabinet.sectionOccupied (rlUnitConfig->cabinetPosition,
                            RLUnitConfig::unitHeight))
                        throw invalid_argument {"RL01/02 cabinet position already occupied"};

                    // The addUnit() shouldn't fail but just to make sure.
                    if (!h9642Cabinet.addUnit (rlUnitConfig->cabinetPosition,
                            RLUnitConfig::unitHeight))
                        throw invalid_argument {"RL01/02 couldn't be added to the cabinet"};
                }
            }
        }
    }
}