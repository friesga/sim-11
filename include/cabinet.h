#ifndef _CABINET_H_
#define _CABINET_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

// The height of the cabinet is expressed in "rack units"(U).
// See https://en.wikipedia.org/wiki/19-inch_rack and 
// https://en.wikipedia.org/wiki/Rack_unit.
//
struct Cabinet
{
    string typeId;
    size_t height;
};

using CabinetConfiguration = vector<Cabinet>;

// Definition of the position of a device in a cabinet. The cabinet number
// is ranking number, the left-most cabinet is cabinet 0. The height of the
// device in the cabinet is expressed in rack units, measured from the top
// of the device. 
//
struct CabinetPosition
{
    size_t cabinetNr;
    size_t height;
};

#endif // _CABINET_H_