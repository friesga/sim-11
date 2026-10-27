#ifndef _MENUOPTIONLIST_H_
#define _MENUOPTIONLIST_H_

#include "../menuoption/menuoption.h"

#include <string>
#include <vector>

// A MenuOptionList defines a list of MenuOption objects that can be used
// to populate a popup menu. The MenuOptionList interface provides methods
// to retrieve the list of option strings and to access individual MenuOption
// objects by index. The vector of strings returned by the strings() method
// can be used to create the popup menu in the browser, while the operator[]
// method can be used to access the selected option.
class MenuOptionList
{
public:
    virtual ~MenuOptionList () = default;
    virtual std::vector<std::string> strings () const = 0;
    virtual size_t size () const = 0;
    virtual const MenuOption& operator[] (std::size_t index) const = 0;
};

#endif // _MENUOPTIONLIST_H_