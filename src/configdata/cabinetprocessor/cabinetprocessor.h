#ifndef _CABINETPROCESSOR_H_
#define _CABINETPROCESSOR_H_

#include "cabinet/cabinet.h"
#include "../iniprocessor/iniparser.h"

#include <string>
#include <sstream>
#include <vector>

using std::string;
using std::stringstream;
using std::getline;
using std::vector;

// This namespace contains functions to process the cabinet key in a
// device configuration. This key can be present for different devices
// hence the separation to a separate namespace.
namespace CabinetProcessor
{

extern Cabinet::Position processCabinetKey (iniparser::Value value);

// Split the given string by the specified delimiter and return the item
// in a vector of the specified type.
template <typename T>
vector<T> split (const string& s, char delim)
{
    vector<T> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim))
    {
        T convertedItem {};
        stringstream {item} >> convertedItem;
        result.push_back (convertedItem);
    }

    return result;
}

} // namespace CabinetProcessor

#endif // _CABINETPROCESSOR_H_