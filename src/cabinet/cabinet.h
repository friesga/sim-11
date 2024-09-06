#ifndef _CABINET_H_
#define _CABINET_H_

#include "rackunit.h"

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::shared_ptr;

// The height of the cabinet is expressed in "rack units"(U).
// See https://en.wikipedia.org/wiki/19-inch_rack and 
// https://en.wikipedia.org/wiki/Rack_unit.
//
class Cabinet
{
public:
    // Definition of the position of a device in a cabinet. The cabinet number
    // is ranking number, the left-most cabinet is cabinet 0. The height of the
    // device in the cabinet is expressed in rack units, measured from the top
    // of the device. 
    //
    struct Position
    {
        size_t cabinetNr;
        RackUnit height;
    };

    Cabinet (string typeId, RackUnit cabinetHeight);
    RackUnit height () const;
    bool addUnit (shared_ptr<Cabinet::Position> position, RackUnit unitHeight);
    bool sectionOutOfRange (shared_ptr<Cabinet::Position> position, RackUnit unitHeight) const;
    bool sectionOccupied (shared_ptr<Cabinet::Position> position, RackUnit unitHeight) const;

private:
    string typeId_;
    RackUnit cabinetHeight_;
    vector<bool> sectionOccupied_;
};

using CabinetConfiguration = vector<Cabinet>;

#endif // _CABINET_H_