#include "cabinet.h"

#include <stdexcept>

using std::invalid_argument;

Cabinet::Cabinet (string typeId, RackUnit cabinetHeight)
    :
    typeId_ {typeId},
    cabinetHeight_ {cabinetHeight}
{
    // Set the sections unoccupied
    for (RackUnit unit = 0_ru; unit < cabinetHeight_; ++unit)
        sectionOccupied_.push_back (false);
}

RackUnit Cabinet::height () const
{
    return cabinetHeight_;
}

// The unit position refers to the position of the top of the unit.
bool Cabinet::addUnit (shared_ptr<Cabinet::Position> unitPosition,
    RackUnit unitHeight)
{
    if (sectionOutOfRange (unitPosition, unitHeight) ||
            sectionOccupied (unitPosition, unitHeight))
        return false;

    for (RackUnit unit = unitPosition->height;
            unit > unitPosition->height - unitHeight; --unit)
        sectionOccupied_[unit] = true;

    return true;
}
bool Cabinet::sectionOutOfRange (shared_ptr<Cabinet::Position> position,
    RackUnit unitHeight) const
{ 
    return (position->height >= cabinetHeight_ ||
        position->height - unitHeight + 1 < 0_ru ||
        position->cabinetNr != 0);
}

bool Cabinet::sectionOccupied (shared_ptr<Cabinet::Position> position,
    RackUnit unitHeight) const
{
    if (sectionOutOfRange (position, unitHeight))
        throw invalid_argument ("Reference outside the cabinet");

    for (RackUnit unit = position->height; 
        unit > position->height - unitHeight; --unit)
    {
        if (sectionOccupied_[unit])
            return true;
    }

    return false;
}
